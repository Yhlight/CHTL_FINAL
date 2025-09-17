#include "Parser.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/AttributeNode.h"
#include "../CHTLNode/LiteralNode.h"
#include "../CHTLNode/TemplateNode.h"
#include "../CHTLNode/TemplateInstantiationNode.h"
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
        auto stmt = parseStatement();
        if (stmt) {
            programNode->children.push_back(std::move(stmt));
        } else {
             if (m_currentToken.type != TokenType::END_OF_FILE) nextToken();
        }
    }
    return programNode;
}

std::unique_ptr<Statement> Parser::parseStatement() {
    switch (m_currentToken.type) {
        case TokenType::IDENTIFIER:
            return parseElementStatement();
        case TokenType::TEMPLATE:
        case TokenType::CUSTOM:
            return parseTemplateStatement();
        case TokenType::AT_ELEMENT:
        case TokenType::AT_STYLE:
            return parseTemplateInstantiationStatement();
        default:
            return nullptr;
    }
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
        std::unique_ptr<Statement> stmt = nullptr;
        if (m_currentToken.type == TokenType::IDENTIFIER) {
            stmt = parseAttribute();
        } else if (m_currentToken.type == TokenType::AT_STYLE) {
            stmt = parseTemplateInstantiationStatement();
        }

        if (stmt) {
            styleNode->children.push_back(std::move(stmt));
        } else {
            nextToken();
        }
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
    m_context.registerTemplate(nameToken.literal, templateNode.get());

    if (!expectPeek(TokenType::LBRACE)) return nullptr;

    if (typeToken.type == TokenType::AT_STYLE) {
        while (m_peekToken.type != TokenType::RBRACE && m_peekToken.type != TokenType::END_OF_FILE) {
            nextToken();
            if (m_currentToken.type == TokenType::IDENTIFIER) {
                templateNode->children.push_back(parseAttribute());
            }
        }
    } else {
        while (m_peekToken.type != TokenType::RBRACE && m_peekToken.type != TokenType::END_OF_FILE) {
            nextToken();
            auto stmt = parseStatement();
            if (stmt) {
                templateNode->children.push_back(std::move(stmt));
            }
        }
    }

    if (!expectPeek(TokenType::RBRACE)) return nullptr;
    return templateNode;
}

std::unique_ptr<Statement> Parser::parseTemplateInstantiationStatement() {
    Token typeToken = m_currentToken;
    if (!expectPeek(TokenType::IDENTIFIER)) return nullptr;
    Token nameToken = m_currentToken;
    auto node = std::make_unique<TemplateInstantiationNode>(typeToken, typeToken, nameToken);
    if (m_peekToken.type == TokenType::SEMICOLON) {
        nextToken();
    }
    return node;
}

std::unique_ptr<ElementNode> Parser::parseElementStatement() {
    auto element = std::make_unique<ElementNode>(m_currentToken, m_currentToken.literal);
    if (!expectPeek(TokenType::LBRACE)) return nullptr;

    while (m_peekToken.type != TokenType::RBRACE && m_peekToken.type != TokenType::END_OF_FILE) {
        nextToken();

        if (m_currentToken.literal == "style" && m_peekToken.type == TokenType::LBRACE) {
            element->styleBlock = parseStyleBlock();
        } else if (m_currentToken.type == TokenType::IDENTIFIER && m_peekToken.type == TokenType::COLON) {
            element->attributes.push_back(parseAttribute());
        } else if (m_currentToken.type == TokenType::TEXT && m_peekToken.type == TokenType::COLON) {
            nextToken();
            nextToken();
            element->children.push_back(std::make_unique<TextNode>(m_currentToken, m_currentToken.literal));
            if (m_peekToken.type == TokenType::SEMICOLON) nextToken();
        } else {
            auto stmt = parseStatement();
            if (stmt) {
                element->children.push_back(std::move(stmt));
            }
        }
    }

    if (!expectPeek(TokenType::RBRACE)) return nullptr;
    return element;
}

} // namespace CHTL
