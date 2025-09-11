#include "CHTLParser.h"
#include "../CHTLNode/AttributeNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/StyleSelectorNode.h"
#include "../CHTLNode/LiteralExpressionNode.h"
#include "../CHTLNode/InfixExpressionNode.h"
#include "../CHTLNode/ConditionalExpressionNode.h"
#include "../CHTLNode/ImportNode.h"
#include "../CHTLNode/ScriptNode.h"
#include <iostream>

namespace CHTL {

const std::map<TokenType, Precedence> CHTLParser::m_precedences = {};

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
        if (peekTokenIs(TokenType::TOKEN_IDENTIFIER) && m_peekToken.literal == "Import") {
            return parseImportStatement();
        }
        return parseTemplateDefinition();
    }
    if (currentTokenIs(TokenType::TOKEN_IDENTIFIER)) return parseElementNode();
    return nullptr;
}

std::shared_ptr<ImportNode> CHTLParser::parseImportStatement() {
    nextToken();
    if (!currentTokenIs(TokenType::TOKEN_IDENTIFIER) || m_currentToken.literal != "Import") return nullptr;
    if (!expectPeek(TokenType::TOKEN_RBRACKET)) return nullptr;
    if (!expectPeek(TokenType::TOKEN_AT)) return nullptr;
    if (!expectPeek(TokenType::TOKEN_IDENTIFIER) || m_currentToken.literal != "Chtl") return nullptr;
    if (!expectPeek(TokenType::TOKEN_IDENTIFIER) || m_currentToken.literal != "from") return nullptr;
    if (!peekTokenIs(TokenType::TOKEN_IDENTIFIER) && !peekTokenIs(TokenType::TOKEN_STRING)) {
        pushError("Expected path for import");
        return nullptr;
    }
    nextToken();
    std::string path = m_currentToken.literal;
    if (peekTokenIs(TokenType::TOKEN_SEMICOLON)) nextToken();
    return std::make_shared<ImportNode>(ImportType::CHTL_FILE, path);
}

std::shared_ptr<TemplateDefinitionNode> CHTLParser::parseTemplateDefinition() {
    nextToken();
    if (!currentTokenIs(TokenType::TOKEN_IDENTIFIER) || m_currentToken.literal != "Template") return nullptr;
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
        NodePtr node = nullptr;
        if (type == TemplateType::STYLE) {
            std::string key = m_currentToken.literal;
            if (!expectPeek(TokenType::TOKEN_COLON)) return nullptr;
            nextToken();
            node = std::make_shared<CSSPropertyNode>(key, parseExpression(LOWEST));
            if (peekTokenIs(TokenType::TOKEN_SEMICOLON)) nextToken();
        } else if (type == TemplateType::ELEMENT) {
            node = parseElementNode();
        }
        if (node) body.push_back(node);
    }
    if (!expectPeek(TokenType::TOKEN_RBRACE)) return nullptr;
    return std::make_shared<TemplateDefinitionNode>(type, name, body);
}

NodePtr CHTLParser::parseTemplateUsage() {
    nextToken();
    if (!currentTokenIs(TokenType::TOKEN_IDENTIFIER)) return nullptr;
    TemplateType type;
    if (m_currentToken.literal == "Style") type = TemplateType::STYLE;
    else if (m_currentToken.literal == "Element") type = TemplateType::ELEMENT;
    else return nullptr;
    if (!expectPeek(TokenType::TOKEN_IDENTIFIER)) return nullptr;
    std::string name = m_currentToken.literal;
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

std::shared_ptr<TextNode> CHTLParser::parseTextNodeShorthand() {
    if (!currentTokenIs(TokenType::TOKEN_COLON) && !currentTokenIs(TokenType::TOKEN_ASSIGN)) return nullptr;
    nextToken();
    std::string textContent = m_currentToken.literal;
    return std::make_shared<TextNode>(textContent);
}

std::shared_ptr<StyleNode> CHTLParser::parseStyleNode() {
    auto styleNode = std::make_shared<StyleNode>();
    if (!expectPeek(TokenType::TOKEN_LBRACE)) return nullptr;
    while (!peekTokenIs(TokenType::TOKEN_RBRACE) && !peekTokenIs(TokenType::TOKEN_EOF)) {
        nextToken();
        if (currentTokenIs(TokenType::TOKEN_AT)) {
            auto usage = parseTemplateUsage();
            if(usage) styleNode->template_usages.push_back(usage);
            if (peekTokenIs(TokenType::TOKEN_SEMICOLON)) nextToken();
        } else if (currentTokenIs(TokenType::TOKEN_IDENTIFIER)) {
            std::string key = m_currentToken.literal;
            if (!expectPeek(TokenType::TOKEN_COLON)) continue;
            nextToken();
            auto value = parseExpression(LOWEST);
            if (value) styleNode->inline_properties.push_back(std::make_shared<CSSPropertyNode>(key, value));
        }
    }
    if (!expectPeek(TokenType::TOKEN_RBRACE)) return nullptr;
    return styleNode;
}

std::shared_ptr<ScriptNode> CHTLParser::parseScriptNode() {
    if (!expectPeek(TokenType::TOKEN_LBRACE)) return nullptr;

    const auto& source = m_lexer.GetInput();
    size_t start_pos = m_lexer.GetReadPosition();

    int brace_level = 1;
    size_t current_pos = start_pos;

    while (brace_level > 0 && current_pos < source.length()) {
        if (source[current_pos] == '{') {
            brace_level++;
        } else if (source[current_pos] == '}') {
            brace_level--;
        }
        current_pos++;
    }

    if (brace_level != 0) {
        pushError("Unterminated script block");
        return nullptr;
    }

    // We have found the closing brace. The content is between start_pos and the char before the closing brace.
    std::string content = source.substr(start_pos, current_pos - start_pos - 1);

    // Now, we need to advance the lexer past this block.
    // This is tricky. A simple way is to just re-lex until we're past it.
    size_t end_offset = current_pos -1;
    while(m_lexer.GetPosition() < end_offset) {
        nextToken();
    }

    return std::make_shared<ScriptNode>(content);
}

std::shared_ptr<ElementNode> CHTLParser::parseElementNode() {
    auto element = std::make_shared<ElementNode>(m_currentToken.literal);
    if (!expectPeek(TokenType::TOKEN_LBRACE)) return nullptr;
    while (!peekTokenIs(TokenType::TOKEN_RBRACE) && !peekTokenIs(TokenType::TOKEN_EOF)) {
        nextToken();
        if (currentTokenIs(TokenType::TOKEN_AT)) {
            auto usage = parseTemplateUsage();
            if(usage) element->children.push_back(usage);
            if (peekTokenIs(TokenType::TOKEN_SEMICOLON)) nextToken();
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
