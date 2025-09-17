#include "Parser.h"
#include <iostream>

namespace CHTL {

Parser::Parser(Lexer& l) : m_lexer(l) {
    // Initialize the token stream by getting the first two tokens
    nextToken();
    nextToken();
}

void Parser::nextToken() {
    m_currentToken = m_peekToken;
    m_peekToken = m_lexer.nextToken();
}

const std::vector<std::string>& Parser::getErrors() const {
    return m_errors;
}

void Parser::peekError(TokenType t) {
    std::string error_msg = "Error: Expected next token to be " + token_type_to_string.at(t) +
                           ", got " + token_type_to_string.at(m_peekToken.type) + " instead.";
    m_errors.push_back(error_msg);
    std::cerr << error_msg << std::endl;
}

bool Parser::currentTokenIs(TokenType t) const {
    return m_currentToken.type == t;
}

bool Parser::peekTokenIs(TokenType t) const {
    return m_peekToken.type == t;
}

bool Parser::expectPeek(TokenType t) {
    if (peekTokenIs(t)) {
        nextToken();
        return true;
    } else {
        peekError(t);
        return false;
    }
}

std::unique_ptr<RootNode> Parser::parseProgram() {
    auto program = std::make_unique<RootNode>();
    parseStatementList(program.get());
    return program;
}

void Parser::parseStatementList(Node* parent) {
    while (!currentTokenIs(TokenType::RIGHT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
        auto stmt = parseStatement();
        if (stmt) {
            parent->addChild(std::move(stmt));
        }
        nextToken();
    }
}

std::unique_ptr<Node> Parser::parseStatement() {
    if (currentTokenIs(TokenType::IDENTIFIER)) {
        return parseElementStatement();
    } else if (currentTokenIs(TokenType::KEYWORD_TEXT)) {
        return parseTextStatement();
    }
    return nullptr;
}

std::unique_ptr<ElementNode> Parser::parseElementStatement() {
    auto element = std::make_unique<ElementNode>(m_currentToken.literal);

    if (!expectPeek(TokenType::LEFT_BRACE)) {
        return nullptr;
    }
    nextToken(); // Consume '{'

    // In here, we can have attributes or child elements.
    while (!currentTokenIs(TokenType::RIGHT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
        if (currentTokenIs(TokenType::IDENTIFIER) && peekTokenIs(TokenType::COLON)) {
            parseAttributes(element.get());
        } else {
             auto stmt = parseStatement();
             if (stmt) {
                 element->addChild(std::move(stmt));
             }
        }
        nextToken();
    }

    return element;
}

void Parser::parseAttributes(ElementNode* element) {
    // Expecting: IDENTIFIER : (STRING | IDENTIFIER) ;
    std::string key = m_currentToken.literal;

    if (!expectPeek(TokenType::COLON)) return;
    nextToken(); // consume ':'

    // CHTL allows unquoted literals for attribute values, which the lexer will
    // often tokenize as IDENTIFIER. We also accept STRING.
    if (currentTokenIs(TokenType::STRING) || currentTokenIs(TokenType::IDENTIFIER)) {
        element->setAttribute(key, m_currentToken.literal);
    } else {
        // Error: invalid attribute value
        std::string error_msg = "Error: Invalid attribute value for key '" + key + "'.";
        m_errors.push_back(error_msg);
        std::cerr << error_msg << std::endl;
    }

    if (peekTokenIs(TokenType::SEMICOLON)) {
        nextToken(); // consume ';'
    }
}

std::unique_ptr<TextNode> Parser::parseTextStatement() {
    // Expecting: text { STRING }
    if (!expectPeek(TokenType::LEFT_BRACE)) {
        return nullptr;
    }
    nextToken(); // consume '{'

    if (!currentTokenIs(TokenType::STRING)) {
         // Error: text block must contain a string literal.
        return nullptr;
    }

    auto textNode = std::make_unique<TextNode>(m_currentToken.literal);

    if (!expectPeek(TokenType::RIGHT_BRACE)) {
        return nullptr;
    }

    return textNode;
}

} // namespace CHTL
