#include "CHTLParser.h"
#include "../CHTLNode/AttributeNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/StyleSelectorNode.h"
#include "../CHTLNode/LiteralExpressionNode.h"
#include "../CHTLNode/InfixExpressionNode.h"
#include "../CHTLNode/ConditionalExpressionNode.h"
#include "../CHTLNode/ImportNode.h"
#include <iostream>

namespace CHTL {

const std::map<TokenType, Precedence> CHTLParser::m_precedences = {
    {TokenType::TOKEN_QUESTION, TERNARY}, {TokenType::TOKEN_PLUS, SUM},
    {TokenType::TOKEN_MINUS, SUM},        {TokenType::TOKEN_GT, SUM},
    {TokenType::TOKEN_LT, SUM},           {TokenType::TOKEN_SLASH, PRODUCT},
    {TokenType::TOKEN_STAR, PRODUCT},     {TokenType::TOKEN_PERCENT, PRODUCT},
    {TokenType::TOKEN_POWER, PRODUCT}};

CHTLParser::CHTLParser(CHTLLexer& lexer) : m_lexer(lexer) {
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

Precedence CHTLParser::currentPrecedence() {
    if (m_precedences.count(m_currentToken.type)) return m_precedences.at(m_currentToken.type);
    return LOWEST;
}

Precedence CHTLParser::peekPrecedence() {
    if (m_precedences.count(m_peekToken.type)) return m_precedences.at(m_peekToken.type);
    return LOWEST;
}

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
    pushError("Unknown token at top level");
    return nullptr;
}

std::shared_ptr<ImportNode> CHTLParser::parseImportStatement() {
    if (!expectPeek(TokenType::TOKEN_IDENTIFIER) || m_currentToken.literal != "Import") return nullptr;
    if (!expectPeek(TokenType::TOKEN_RBRACKET)) return nullptr;
    if (!expectPeek(TokenType::TOKEN_AT)) return nullptr;
    if (!expectPeek(TokenType::TOKEN_IDENTIFIER) || m_currentToken.literal != "Chtl") return nullptr;

    if (!expectPeek(TokenType::TOKEN_IDENTIFIER) || m_currentToken.literal != "from") return nullptr;

    if (!peekTokenIs(TokenType::TOKEN_IDENTIFIER) && !peekTokenIs(TokenType::TOKEN_STRING)) {
        pushError("Expected path (string or identifier) for import statement");
        return nullptr;
    }
    nextToken(); // consume path token
    std::string path = m_currentToken.literal;

    if (peekTokenIs(TokenType::TOKEN_SEMICOLON)) nextToken();

    return std::make_shared<ImportNode>(ImportType::CHTL_FILE, path);
}

std::shared_ptr<TemplateDefinitionNode> CHTLParser::parseTemplateDefinition() {
    if (!expectPeek(TokenType::TOKEN_IDENTIFIER) || m_currentToken.literal != "Template") return nullptr;
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
            if (currentTokenIs(TokenType::TOKEN_IDENTIFIER)) {
                std::string key = m_currentToken.literal;
                if (!expectPeek(TokenType::TOKEN_COLON)) return nullptr;
                nextToken();
                node = std::make_shared<CSSPropertyNode>(key, parseExpression(LOWEST));
                if (peekTokenIs(TokenType::TOKEN_SEMICOLON)) nextToken();
            }
        } else if (type == TemplateType::ELEMENT) {
            node = parseElementNode();
        }
        if (node) body.push_back(node);
    }
    if (!expectPeek(TokenType::TOKEN_RBRACE)) return nullptr;
    return std::make_shared<TemplateDefinitionNode>(type, name, body);
}

NodePtr CHTLParser::parseTemplateUsage() {
    if (!expectPeek(TokenType::TOKEN_IDENTIFIER)) return nullptr;
    TemplateType type;
    if (m_currentToken.literal == "Style") type = TemplateType::STYLE;
    else if (m_currentToken.literal == "Element") type = TemplateType::ELEMENT;
    else return nullptr;
    if (!expectPeek(TokenType::TOKEN_IDENTIFIER)) return nullptr;
    std::string name = m_currentToken.literal;
    if (peekTokenIs(TokenType::TOKEN_SEMICOLON)) nextToken();
    return std::make_shared<TemplateUsageNode>(type, name);
}

ExpressionPtr CHTLParser::parseExpression(Precedence precedence) {
    std::string literal_value = "";
    while (!currentTokenIs(TokenType::TOKEN_SEMICOLON) && !currentTokenIs(TokenType::TOKEN_RBRACE) && !currentTokenIs(TokenType::TOKEN_EOF)) {
        literal_value += m_currentToken.literal;
        if (!peekTokenIs(TokenType::TOKEN_SEMICOLON) && !peekTokenIs(TokenType::TOKEN_RBRACE)) {
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
    if (peekTokenIs(TokenType::TOKEN_SEMICOLON)) nextToken();
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
        } else if (currentTokenIs(TokenType::TOKEN_IDENTIFIER)) {
            std::string key = m_currentToken.literal;
            if (!expectPeek(TokenType::TOKEN_COLON)) continue;
            nextToken();
            auto value = parseExpression(LOWEST);
            if (value) styleNode->inline_properties.push_back(std::make_shared<CSSPropertyNode>(key, value));
            if (currentTokenIs(TokenType::TOKEN_SEMICOLON)) {
            } else {
            }
        } else {
            pushError("Invalid token in style block");
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
            auto usage = parseTemplateUsage();
            if(usage) element->children.push_back(usage);
        } else if (currentTokenIs(TokenType::TOKEN_IDENTIFIER)) {
            std::string key = m_currentToken.literal;
            if (key == "style") {
                auto style = parseStyleNode();
                if(style) element->children.push_back(style);
            } else if (peekTokenIs(TokenType::TOKEN_COLON) || peekTokenIs(TokenType::TOKEN_ASSIGN)) {
                nextToken();
                if (key == "text") {
                    auto text = parseTextNodeShorthand();
                    if(text) element->children.push_back(text);
                } else {
                    nextToken();
                    element->attributes.push_back(std::make_shared<AttributeNode>(key, m_currentToken.literal));
                    if (peekTokenIs(TokenType::TOKEN_SEMICOLON)) nextToken();
                }
            } else if (peekTokenIs(TokenType::TOKEN_LBRACE)) {
                auto child_element = parseElementNode();
                if(child_element) element->children.push_back(child_element);
            } else {
                 pushError("Unexpected identifier in element body");
            }
        } else {
            pushError("Unexpected token in element body");
        }
    }
    if (!expectPeek(TokenType::TOKEN_RBRACE)) return nullptr;
    return element;
}

}
