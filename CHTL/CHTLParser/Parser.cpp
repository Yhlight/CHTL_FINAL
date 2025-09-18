#include "Parser.hpp"
#include <iostream> // For debugging

namespace CHTL {

Parser::Parser(Lexer& l) : lexer(l) {
    // Prime the parser with the first two tokens
    nextToken();
    nextToken();
}

void Parser::nextToken() {
    currentToken = peekToken;
    peekToken = lexer.nextToken();
}

const std::vector<std::string>& Parser::getErrors() const {
    return errors;
}

std::unique_ptr<Program> Parser::parseProgram() {
    auto program = std::make_unique<Program>();

    while (currentToken.type != TokenType::END_OF_FILE) {
        auto node = parseNode();
        if (node) {
            program->addChild(std::move(node));
        }
        nextToken();
    }

    return program;
}

std::unique_ptr<Node> Parser::parseNode() {
    if (currentToken.type == TokenType::IDENT) {
        return parseElementNode();
    }
    // No other node types at the root level for now
    return nullptr;
}

std::unique_ptr<ElementNode> Parser::parseElementNode() {
    auto elementNode = std::make_unique<ElementNode>(currentToken.literal);

    // Special handling for the 'text' element
    if (elementNode->tagName == "text") {
        if (peekToken.type != TokenType::LBRACE) {
            errors.push_back("Expected '{' after 'text' identifier.");
            return nullptr;
        }
        nextToken(); // Consume 'text'
        nextToken(); // Consume '{'

        auto textContentNode = parseTextNode();
        if (textContentNode) {
             elementNode->addChild(std::move(textContentNode));
        }

        if (currentToken.type != TokenType::RBRACE) {
            errors.push_back("Expected '}' after text block.");
            return nullptr;
        }
        // The nextToken in the main loop will consume the '}'
        return elementNode;
    }

    if (peekToken.type != TokenType::LBRACE) {
        errors.push_back("Expected '{' after element identifier.");
        return nullptr;
    }
    nextToken(); // Consume identifier
    nextToken(); // Consume '{'

    // Parse child nodes
    while (currentToken.type != TokenType::RBRACE && currentToken.type != TokenType::END_OF_FILE) {
        auto childNode = parseNode();
        if (childNode) {
            elementNode->addChild(std::move(childNode));
        }
        nextToken();
    }

    if (currentToken.type != TokenType::RBRACE) {
        errors.push_back("Expected '}' to close element block.");
        return nullptr;
    }

    return elementNode;
}

std::unique_ptr<TextNode> Parser::parseTextNode() {
    std::string textContent;

    if (currentToken.type == TokenType::STRING) {
        textContent = currentToken.literal;
        nextToken(); // consume string literal
    } else {
        // Handle unquoted literals by concatenating identifiers
        while (currentToken.type == TokenType::IDENT) {
            textContent += currentToken.literal;
            if (peekToken.type == TokenType::IDENT) {
                textContent += " "; // Add space between identifiers
            }
            nextToken();
        }
    }

    return std::make_unique<TextNode>(textContent);
}

} // namespace CHTL
