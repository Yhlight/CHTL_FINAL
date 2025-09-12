#include "CHTLParser.h"
#include "../CHTLNode/AttributeNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/StyleSelectorNode.h"
#include "../CHTLNode/LiteralExpressionNode.h"
#include "../CHTLNode/InfixExpressionNode.h"
#include "../CHTLNode/ConditionalExpressionNode.h"
#include "../CHTLNode/ImportNode.h"
#include "../CHTLNode/ScriptNode.h"
#include "../CHTLNode/CustomDefinitionNode.h"
#include "../CHTLNode/CustomUsageNode.h"
#include "../CHTLNode/DeletePropertyNode.h"
#include <iostream>

namespace CHTL {

const std::map<std::string, TokenType> CHTLParser::m_keywords = {
    {"Template", TokenType::TOKEN_KEYWORD_TEMPLATE},
    {"Custom", TokenType::TOKEN_KEYWORD_CUSTOM},
    {"Import", TokenType::TOKEN_KEYWORD_IMPORT},
    {"delete", TokenType::TOKEN_KEYWORD_DELETE},
    {"insert", TokenType::TOKEN_KEYWORD_INSERT},
    {"after", TokenType::TOKEN_KEYWORD_AFTER},
    {"before", TokenType::TOKEN_KEYWORD_BEFORE},
    {"replace", TokenType::TOKEN_KEYWORD_REPLACE},
    {"from", TokenType::TOKEN_KEYWORD_FROM},
    {"as", TokenType::TOKEN_KEYWORD_AS},
};

TokenType CHTLParser::lookupIdent(const std::string& ident) {
    if (m_keywords.count(ident)) {
        return m_keywords.at(ident);
    }
    return TokenType::TOKEN_IDENTIFIER;
}

CHTLParser::CHTLParser(CHTLLexer& lexer, bool debug_mode) : m_lexer(lexer), m_debug_mode(debug_mode) {
    nextToken();
    nextToken();
}

void CHTLParser::nextToken() {
    m_currentToken = m_peekToken;
    m_peekToken = m_lexer.NextToken();
}

bool CHTLParser::currentTokenIs(TokenType type) const { return m_currentToken.type == type; }
bool CHTLParser::peekTokenIs(TokenType type) const { return m_peekToken.type == type; }

void CHTLParser::pushError(const std::string& msg) {
    m_errors.push_back(msg + " (current: " + m_currentToken.literal + ", peek: " + m_peekToken.literal + ")");
}

bool CHTLParser::expectPeek(TokenType type) {
    if (peekTokenIs(type)) {
        nextToken();
        return true;
    }
    pushError("Expected next token to be " + std::to_string((int)type));
    return false;
}

Precedence CHTLParser::currentPrecedence() { return LOWEST; }
Precedence CHTLParser::peekPrecedence() { return LOWEST; }

std::shared_ptr<ProgramNode> CHTLParser::ParseProgram() {
    auto program = std::make_shared<ProgramNode>();
    while (!currentTokenIs(TokenType::TOKEN_EOF)) {
        auto stmt = parseStatement();
        if (stmt) {
            program->statements.push_back(stmt);
        } else {
            nextToken();
        }
    }
    return program;
}

NodePtr CHTLParser::parseStatement() {
    if (currentTokenIs(TokenType::TOKEN_LBRACKET)) {
        if (peekTokenIs(TokenType::TOKEN_KEYWORD_IMPORT)) return parseImportStatement();
        if (peekTokenIs(TokenType::TOKEN_KEYWORD_TEMPLATE)) return parseTemplateDefinition();
        if (peekTokenIs(TokenType::TOKEN_KEYWORD_CUSTOM)) return parseCustomDefinition();
    }
    if (currentTokenIs(TokenType::TOKEN_IDENTIFIER)) return parseElementNode();
    return nullptr;
}

std::shared_ptr<ImportNode> CHTLParser::parseImportStatement() { return nullptr; }

std::shared_ptr<CustomDefinitionNode> CHTLParser::parseCustomDefinition() {
    nextToken();
    if (!currentTokenIs(TokenType::TOKEN_KEYWORD_CUSTOM)) return nullptr;
    if (!expectPeek(TokenType::TOKEN_RBRACKET)) return nullptr;
    if (!expectPeek(TokenType::TOKEN_AT)) return nullptr;
    if (!expectPeek(TokenType::TOKEN_IDENTIFIER)) return nullptr;

    TemplateType type;
    if (m_currentToken.literal == "Style") type = TemplateType::STYLE;
    else if (m_currentToken.literal == "Element") type = TemplateType::ELEMENT;
    else return nullptr;

    if (!expectPeek(TokenType::TOKEN_IDENTIFIER)) return nullptr;
    std::string name = m_currentToken.literal;
    if (!expectPeek(TokenType::TOKEN_LBRACE)) return nullptr;

    NodeList body;
    while (!peekTokenIs(TokenType::TOKEN_RBRACE) && !peekTokenIs(TokenType::TOKEN_EOF)) {
        nextToken();
        if (currentTokenIs(TokenType::TOKEN_IDENTIFIER)) {
            std::string key = m_currentToken.literal;
            if (peekTokenIs(TokenType::TOKEN_COMMA) || peekTokenIs(TokenType::TOKEN_SEMICOLON)) {
                body.push_back(std::make_shared<CSSPropertyNode>(key, nullptr));
                nextToken();
            }
            else if (peekTokenIs(TokenType::TOKEN_COLON)) {
                nextToken();
                nextToken();
                body.push_back(std::make_shared<CSSPropertyNode>(key, parseExpression(LOWEST)));
                if (peekTokenIs(TokenType::TOKEN_SEMICOLON)) nextToken();
            }
        }
    }
    if (!expectPeek(TokenType::TOKEN_RBRACE)) return nullptr;
    return std::make_shared<CustomDefinitionNode>(type, name, body);
}

std::shared_ptr<TemplateDefinitionNode> CHTLParser::parseTemplateDefinition() { return nullptr; }

NodePtr CHTLParser::parseTemplateOrCustomUsage() {
    nextToken();
    if (!currentTokenIs(TokenType::TOKEN_IDENTIFIER)) return nullptr;

    TemplateType type;
    if (m_currentToken.literal == "Style") type = TemplateType::STYLE;
    else if (m_currentToken.literal == "Element") type = TemplateType::ELEMENT;
    else return nullptr;

    if (!expectPeek(TokenType::TOKEN_IDENTIFIER)) return nullptr;
    std::string name = m_currentToken.literal;

    NodeList body;
    if (peekTokenIs(TokenType::TOKEN_LBRACE)) {
        nextToken();
        while (!peekTokenIs(TokenType::TOKEN_RBRACE) && !peekTokenIs(TokenType::TOKEN_EOF)) {
            nextToken();
            if (currentTokenIs(TokenType::TOKEN_KEYWORD_DELETE)) {
                nextToken();
                body.push_back(std::make_shared<DeletePropertyNode>(m_currentToken.literal));
                if(peekTokenIs(TokenType::TOKEN_SEMICOLON)) nextToken();
            }
            else if (currentTokenIs(TokenType::TOKEN_IDENTIFIER)) {
                std::string key = m_currentToken.literal;
                if (!expectPeek(TokenType::TOKEN_COLON)) continue;
                nextToken();
                body.push_back(std::make_shared<CSSPropertyNode>(key, parseExpression(LOWEST)));
                 if (peekTokenIs(TokenType::TOKEN_SEMICOLON)) nextToken();
            }
        }
        if (!expectPeek(TokenType::TOKEN_RBRACE)) return nullptr;
        return std::make_shared<CustomUsageNode>(type, name, body);
    }

    if (peekTokenIs(TokenType::TOKEN_SEMICOLON)) nextToken();
    return std::make_shared<TemplateUsageNode>(type, name);
}

ExpressionPtr CHTLParser::parseExpression(Precedence precedence) {
    std::string literal_value;
    while (!currentTokenIs(TokenType::TOKEN_SEMICOLON) && !currentTokenIs(TokenType::TOKEN_RBRACE) && !currentTokenIs(TokenType::TOKEN_EOF)) {
        literal_value += m_currentToken.literal;
        if (!peekTokenIs(TokenType::TOKEN_SEMICOLON) && !peekTokenIs(TokenType::TOKEN_RBRACE) && !peekTokenIs(TokenType::TOKEN_EOF)) {
            literal_value += " ";
        }
        nextToken();
    }
    if (!literal_value.empty() && literal_value.back() == ' ') {
        literal_value.pop_back();
    }
    Token t;
    t.literal = literal_value;
    t.type = TokenType::TOKEN_STRING;
    return std::make_shared<LiteralExpressionNode>(t);
}

ExpressionPtr CHTLParser::parseIdentifierAsExpression() { return nullptr; }
ExpressionPtr CHTLParser::parseInfixExpression(ExpressionPtr left) { return nullptr; }
ExpressionPtr CHTLParser::parseConditionalExpression(ExpressionPtr condition) { return nullptr; }
std::shared_ptr<TextNode> CHTLParser::parseTextNodeShorthand() { return nullptr; }
std::shared_ptr<ScriptNode> CHTLParser::parseScriptNode() { return nullptr; }

std::shared_ptr<StyleNode> CHTLParser::parseStyleNode() {
    auto styleNode = std::make_shared<StyleNode>();
    if (!expectPeek(TokenType::TOKEN_LBRACE)) return nullptr;
    while (!peekTokenIs(TokenType::TOKEN_RBRACE) && !peekTokenIs(TokenType::TOKEN_EOF)) {
        nextToken();
        if (currentTokenIs(TokenType::TOKEN_AT)) {
            auto usage = parseTemplateOrCustomUsage();
            if(usage) styleNode->template_usages.push_back(usage);
        } else if (currentTokenIs(TokenType::TOKEN_IDENTIFIER)) {
            std::string key = m_currentToken.literal;
            if (!expectPeek(TokenType::TOKEN_COLON)) continue;
            nextToken();
            auto value = parseExpression(LOWEST);
            if (value) styleNode->inline_properties.push_back(std::make_shared<CSSPropertyNode>(key, value));
            if (peekTokenIs(TokenType::TOKEN_SEMICOLON)) nextToken();
        }
    }
    if (!expectPeek(TokenType::TOKEN_RBRACE)) return nullptr;
    return styleNode;
}

std::shared_ptr<ElementNode> CHTLParser::parseElementNode() {
    auto element = std::make_shared<ElementNode>(m_currentToken.literal);
    if (!expectPeek(TokenType::TOKEN_LBRACE)) return nullptr;
    while (!peekTokenIs(TokenType::TOKEN_RBRACE) && !peekTokenIs(TokenType::TOKEN_EOF)) {
        nextToken();
        if (currentTokenIs(TokenType::TOKEN_AT)) {
            auto usage = parseTemplateOrCustomUsage();
            if(usage) element->children.push_back(usage);
        } else if (currentTokenIs(TokenType::TOKEN_IDENTIFIER)) {
            std::string key = m_currentToken.literal;
            if (key == "style") {
                auto style = parseStyleNode();
                if(style) element->children.push_back(style);
            } else if (key == "script") {
                auto script = parseScriptNode();
                if(script) element->children.push_back(script);
            }
            else if (peekTokenIs(TokenType::TOKEN_COLON) || peekTokenIs(TokenType::TOKEN_ASSIGN)) {
                nextToken();
                if (key == "text") {
                    auto text = parseTextNodeShorthand();
                    if(text) element->children.push_back(text);
                } else {
                    nextToken();
                    element->attributes.push_back(std::make_shared<AttributeNode>(key, m_currentToken.literal));
                }
                if (peekTokenIs(TokenType::TOKEN_SEMICOLON)) nextToken();
            } else if (peekTokenIs(TokenType::TOKEN_LBRACE)) {
                auto child_element = parseElementNode();
                if(child_element) element->children.push_back(child_element);
            }
        }
    }
    if (!expectPeek(TokenType::TOKEN_RBRACE)) return nullptr;
    return element;
}

}
