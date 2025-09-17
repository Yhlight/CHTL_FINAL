#include "Parser.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/AttributeNode.h"
#include "../CHTLNode/LiteralNode.h"
#include "../CHTLNode/TemplateNode.h"
#include <string>

namespace CHTL {

Parser::Parser(Lexer& lexer) : m_lexer(lexer) {
    nextToken();
    nextToken();
}

void Parser::nextToken() {
    m_currentToken = m_peekToken;
    m_peekToken = m_lexer.nextToken();
}

void Parser::peekError(TokenType t) {
    std::string error = "Error on line " + std::to_string(m_peekToken.line) + ": Expected " + std::to_string((int)t) + ", got " + std::to_string((int)m_peekToken.type) + " instead.";
    m_errors.push_back(error);
}

bool Parser::expectPeek(TokenType t) {
    if (m_peekToken.type == t) {
        nextToken();
        return true;
    }
    peekError(t);
    return false;
}

std::unique_ptr<Node> Parser::parseProgram() {
    auto programNode = std::make_unique<ElementNode>(Token{TokenType::IDENTIFIER, "root"}, "root");
    while (m_currentToken.type != TokenType::END_OF_FILE) {
        std::unique_ptr<Statement> stmt = nullptr;
        switch (m_currentToken.type) {
            case TokenType::TEMPLATE:
            case TokenType::CUSTOM:
                stmt = parseTemplateStatement();
                break;
            case TokenType::IDENTIFIER:
                stmt = parseStatement();
                break;
            default:
                // Skip unknown tokens at top level
                nextToken();
                break;
        }

        if (stmt) {
            programNode->children.push_back(std::move(stmt));
        }
    }
    return programNode;
}

std::unique_ptr<Statement> Parser::parseStatement() {
    if (m_currentToken.type == TokenType::IDENTIFIER) {
        return parseElementStatement();
    }
    return nullptr;
}

std::unique_ptr<AttributeNode> Parser::parseAttribute() {
    auto attr = std::make_unique<AttributeNode>(m_currentToken, m_currentToken.literal);
    nextToken();
    nextToken();
    attr->m_value = std::make_unique<LiteralNode>(m_currentToken);
    if (m_peekToken.type == TokenType::SEMICOLON) {
        nextToken();
    }
    return attr;
}

std::unique_ptr<StyleNode> Parser::parseStyleBlock() {
    auto styleNode = std::make_unique<StyleNode>(m_currentToken);
    nextToken();
    nextToken();
    while (m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::END_OF_FILE) {
        if (m_currentToken.type == TokenType::IDENTIFIER) {
            auto prop = parseAttribute();
            if (prop) {
                styleNode->properties.push_back(std::move(prop));
            }
        }
        nextToken();
    }
    return styleNode;
}

std::unique_ptr<TemplateNode> Parser::parseTemplateStatement() {
    Token templateToken = m_currentToken;
    nextToken();
    Token typeToken = m_currentToken;
    nextToken();
    Token nameToken = m_currentToken;

    auto templateNode = std::make_unique<TemplateNode>(templateToken, typeToken, nameToken);

    if (!expectPeek(TokenType::LBRACE)) {
        return nullptr;
    }
    nextToken();

    while (m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::END_OF_FILE) {
        auto stmt = parseStatement();
        if (stmt) {
            templateNode->children.push_back(std::move(stmt));
        } else {
            nextToken();
        }
    }
    nextToken(); // Consume closing brace
    return templateNode;
}

std::unique_ptr<ElementNode> Parser::parseElementStatement() {
    auto element = std::make_unique<ElementNode>(m_currentToken, m_currentToken.literal);
    if (!expectPeek(TokenType::LBRACE)) return nullptr;

    nextToken();

    while (m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::END_OF_FILE) {
        if (m_currentToken.literal == "style" && m_peekToken.type == TokenType::LBRACE) {
            element->styleBlock = parseStyleBlock();
        } else if (m_currentToken.type == TokenType::IDENTIFIER && m_peekToken.type == TokenType::LBRACE) {
            element->children.push_back(parseElementStatement());
        } else if (m_currentToken.type == TokenType::TEXT && m_peekToken.type == TokenType::COLON) {
            nextToken();
            nextToken();
            element->children.push_back(std::make_unique<TextNode>(m_currentToken, m_currentToken.literal));
            if (m_peekToken.type == TokenType::SEMICOLON) nextToken();
        } else if (m_currentToken.type == TokenType::IDENTIFIER && m_peekToken.type == TokenType::COLON) {
             element->attributes.push_back(parseAttribute());
        }
        nextToken();
    }
    return element;
}

} // namespace CHTL
