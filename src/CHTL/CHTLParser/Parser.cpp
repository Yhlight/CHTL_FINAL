#include "Parser.h"
#include <iostream>
#include <string>

Parser::Parser(Lexer& lexer) : lexer(lexer) {
    nextToken();
    nextToken();
}

void Parser::nextToken() {
    currentToken = peekToken;
    peekToken = lexer.getNextToken();
}

const std::vector<std::string>& Parser::getErrors() const {
    return errors;
}

void Parser::peekError(TokenType type) {
    std::string error = "Parser Error: Expected next token to be " + std::to_string((int)type) +
                        ", got " + std::to_string((int)peekToken.type) + " instead. (line: " + std::to_string(peekToken.line) + ")";
    errors.push_back(error);
}

bool Parser::expectPeek(TokenType type) {
    if (peekTokenIs(type)) {
        nextToken();
        return true;
    } else {
        peekError(type);
        return false;
    }
}

NodeList Parser::parse() {
    NodeList nodes;
    while (!currentTokenIs(TokenType::END_OF_FILE)) {
        NodePtr stmt = parseStatement();
        if (stmt) {
            nodes.push_back(std::move(stmt));
        } else {
            // If parseStatement returns nullptr, it's an unhandled token.
            // At the top level, this is a syntax error. Stop parsing.
            // (The error is recorded by other functions like expectPeek).
            // We break here to avoid an infinite loop on an invalid token.
            break;
        }
    }
    return nodes;
}

NodePtr Parser::parseStatement() {
    if (currentTokenIs(TokenType::IDENTIFIER)) {
        if (peekTokenIs(TokenType::L_BRACE)) {
            return parseElement();
        }
    } else if (currentTokenIs(TokenType::KEYWORD_TEXT)) {
        return parseText();
    }
    return nullptr;
}

NodePtr Parser::parseElement() {
    auto element = std::make_unique<ElementNode>(currentToken.value);

    // Current token is the element name (e.g., 'div'). We expect a '{' next.
    if (!expectPeek(TokenType::L_BRACE)) {
        return nullptr;
    }

    // Now currentToken is '{'. We loop, parsing the content inside.
    while (!peekTokenIs(TokenType::R_BRACE) && !peekTokenIs(TokenType::END_OF_FILE)) {
        nextToken(); // Move to the first token of a child statement/attribute

        if (currentTokenIs(TokenType::IDENTIFIER) && (peekTokenIs(TokenType::COLON) || peekTokenIs(TokenType::EQUALS))) {
            AttributeNode attr;
            attr.key = currentToken.value;
            nextToken(); // consume key
            nextToken(); // consume ':' or '='
            attr.value = currentToken.value;
            element->attributes.push_back(attr);
            if (!expectPeek(TokenType::SEMICOLON)) return nullptr; // consumes value, currentToken is now ';'
        } else {
            NodePtr child = parseStatement();
            if (child) {
                element->children.push_back(std::move(child));
            }
        }
    }

    // Expect the final closing brace
    if (!expectPeek(TokenType::R_BRACE)) {
        return nullptr;
    }

    return element;
}

NodePtr Parser::parseText() {
    // Current token is 'text'. We expect a '{' next.
    if (!expectPeek(TokenType::L_BRACE)) {
        return nullptr;
    }

    // Now currentToken is '{'. The content is next.
    nextToken(); // Move to the first token of the content.

    std::string textContent;
    while (!currentTokenIs(TokenType::R_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
        textContent += currentToken.value;
        if (!peekTokenIs(TokenType::R_BRACE)) {
            textContent += " ";
        }
        nextToken();
    }

    // Trim trailing space if any
    if (!textContent.empty() && textContent.back() == ' ') {
        textContent.pop_back();
    }

    // CurrentToken is now '}'. The main loop will advance past it.
    return std::make_unique<TextNode>(textContent);
}

bool Parser::currentTokenIs(TokenType type) {
    return currentToken.type == type;
}

bool Parser::peekTokenIs(TokenType type) {
    return peekToken.type == type;
}
