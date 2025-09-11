#include "CHTLParser.h"
#include "../CHTLNode/AttributeNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/StyleSelectorNode.h"
#include "../CHTLNode/LiteralExpressionNode.h"
#include "../CHTLNode/InfixExpressionNode.h"
#include "../CHTLNode/ConditionalExpressionNode.h"
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

bool CHTLParser::expectPeek(TokenType type) {
    if (peekTokenIs(type)) {
        nextToken();
        return true;
    }
    m_errors.push_back("Expected next token to be " +
                       std::to_string((int)type) + ", got " +
                       std::to_string((int)m_peekToken.type) + " instead.");
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
    if (currentTokenIs(TokenType::TOKEN_LBRACKET)) return parseTemplateDefinition();
    if (currentTokenIs(TokenType::TOKEN_IDENTIFIER)) return parseElementNode();
    return nullptr;
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
        if (type == TemplateType::STYLE) {
            std::string key = m_currentToken.literal;
            if (!expectPeek(TokenType::TOKEN_COLON)) return nullptr;
            nextToken();
            body.push_back(std::make_shared<CSSPropertyNode>(key, parseExpression(LOWEST)));
            if (peekTokenIs(TokenType::TOKEN_SEMICOLON)) nextToken();
        } else if (type == TemplateType::ELEMENT) {
            body.push_back(parseElementNode());
        }
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
    ExpressionPtr leftExp = parseIdentifierAsExpression();
    if (!leftExp) return nullptr;

    while (!peekTokenIs(TokenType::TOKEN_SEMICOLON) && !peekTokenIs(TokenType::TOKEN_RBRACE) && precedence < peekPrecedence()) {
        if (peekTokenIs(TokenType::TOKEN_QUESTION)) {
             nextToken();
             leftExp = parseConditionalExpression(leftExp);
        } else if (m_precedences.count(m_peekToken.type)) {
            nextToken();
            leftExp = parseInfixExpression(leftExp);
        } else {
            return leftExp;
        }
    }
    return leftExp;
}

ExpressionPtr CHTLParser::parseIdentifierAsExpression() {
    if (m_currentToken.type != TokenType::TOKEN_IDENTIFIER && m_currentToken.type != TokenType::TOKEN_STRING) {
        m_errors.push_back("Unexpected token in expression: " + m_currentToken.literal);
        return nullptr;
    }
    return std::make_shared<LiteralExpressionNode>(m_currentToken);
}

ExpressionPtr CHTLParser::parseInfixExpression(ExpressionPtr left) {
    auto op = m_currentToken;
    auto precedence = currentPrecedence();
    nextToken();
    auto right = parseExpression(precedence);
    return std::make_shared<InfixExpressionNode>(left, op, right);
}

ExpressionPtr CHTLParser::parseConditionalExpression(ExpressionPtr condition) {
    nextToken();
    ExpressionPtr consequence = parseExpression(LOWEST);
    if (!expectPeek(TokenType::TOKEN_COLON)) return nullptr;
    nextToken();
    ExpressionPtr alternative = parseExpression(LOWEST);
    return std::make_shared<ConditionalExpressionNode>(condition, consequence, alternative);
}

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
            styleNode->template_usages.push_back(parseTemplateUsage());
        } else if (currentTokenIs(TokenType::TOKEN_IDENTIFIER)) {
            std::string key = m_currentToken.literal;
            if (!expectPeek(TokenType::TOKEN_COLON)) continue;
            nextToken();
            styleNode->inline_properties.push_back(std::make_shared<CSSPropertyNode>(key, parseExpression(LOWEST)));
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
            element->children.push_back(parseTemplateUsage());
        } else if (currentTokenIs(TokenType::TOKEN_IDENTIFIER)) {
            std::string key = m_currentToken.literal;
            if (key == "style") {
                element->children.push_back(parseStyleNode());
            } else if (peekTokenIs(TokenType::TOKEN_COLON) || peekTokenIs(TokenType::TOKEN_ASSIGN)) {
                nextToken();
                if (key == "text") {
                    element->children.push_back(parseTextNodeShorthand());
                } else {
                    nextToken();
                    element->attributes.push_back(std::make_shared<AttributeNode>(key, m_currentToken.literal));
                    if (peekTokenIs(TokenType::TOKEN_SEMICOLON)) nextToken();
                }
            } else if (peekTokenIs(TokenType::TOKEN_LBRACE)) {
                element->children.push_back(parseElementNode());
            }
        }
    }
    if (!expectPeek(TokenType::TOKEN_RBRACE)) return nullptr;
    return element;
}

}
