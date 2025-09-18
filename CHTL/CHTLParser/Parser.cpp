#include "Parser.hpp"
#include <iostream>

namespace CHTL {

Parser::Parser(Lexer& l) : lexer(l) {
    nextToken();
    nextToken();
}

void Parser::nextToken() {
    currentToken = peekToken;
    peekToken = lexer.nextToken();
}

bool Parser::expectPeek(TokenType t) {
    if (peekToken.type == t) {
        nextToken();
        return true;
    }
    return false;
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
        } else {
            nextToken();
        }
    }
    return program;
}

std::unique_ptr<Node> Parser::parseNode() {
    if (currentToken.type == TokenType::COMMENT) {
        auto node = parseCommentNode();
        return node;
    }
    if (currentToken.type == TokenType::IDENT) {
        auto node = parseElementNode();
        return node;
    }
    return nullptr;
}

void Parser::parseAttributes(ElementNode* element) {
    std::string key = currentToken.literal;

    if (!expectPeek(TokenType::COLON) && !expectPeek(TokenType::EQUAL)) {
        errors.push_back("Expected ':' or '=' after attribute name.");
        return;
    }

    nextToken();

    std::string value;
    if (currentToken.type == TokenType::STRING || currentToken.type == TokenType::IDENT) {
        value = currentToken.literal;
    } else {
        errors.push_back("Attribute value must be a string or an identifier.");
        return;
    }

    element->attributes[key] = value;

    if (peekToken.type == TokenType::SEMICOLON) {
        nextToken();
    }
}

void Parser::parseStyleBlock(ElementNode* parent) {
    nextToken(); // consume 'style'
    if (currentToken.type != TokenType::LBRACE) {
        errors.push_back("Expected '{' after 'style' identifier.");
        return;
    }
    nextToken();

    while (currentToken.type != TokenType::RBRACE && currentToken.type != TokenType::END_OF_FILE) {
        if (currentToken.type != TokenType::IDENT) {
            errors.push_back("Expected style property name.");
            break;
        }
        std::string key = currentToken.literal;

        if (!expectPeek(TokenType::COLON)) {
            errors.push_back("Expected ':' after style property name.");
            break;
        }
        nextToken();

        std::string value;
        while (currentToken.type != TokenType::SEMICOLON && currentToken.type != TokenType::RBRACE) {
            value += currentToken.literal;
            nextToken();
        }

        parent->styles[key] = value;

        if (currentToken.type == TokenType::SEMICOLON) {
            nextToken();
        }
    }
}

std::unique_ptr<ElementNode> Parser::parseElementNode() {
    auto elementNode = std::make_unique<ElementNode>(currentToken.literal);
    nextToken();

    if (elementNode->tagName == "text") {
        if (currentToken.type != TokenType::LBRACE) {
            errors.push_back("Expected '{' after 'text' identifier.");
            return nullptr;
        }
        nextToken();

        std::string textContent;
        if (currentToken.type == TokenType::STRING) {
            textContent = currentToken.literal;
            nextToken();
        } else {
            while(currentToken.type != TokenType::RBRACE && currentToken.type != TokenType::END_OF_FILE) {
                textContent += currentToken.literal;
                nextToken();
                if (currentToken.type != TokenType::RBRACE) {
                    textContent += " ";
                }
            }
        }
        auto textNode = std::make_unique<TextNode>(textContent);
        elementNode->addChild(std::move(textNode));

        if (currentToken.type != TokenType::RBRACE) {
            errors.push_back("Expected '}' to close text block.");
            return nullptr;
        }
        return elementNode;
    }

    if (currentToken.type != TokenType::LBRACE) {
        errors.push_back("Expected '{' after element identifier.");
        return nullptr;
    }
    nextToken();

    while (currentToken.type != TokenType::RBRACE && currentToken.type != TokenType::END_OF_FILE) {
        if (currentToken.type == TokenType::COMMENT) {
            auto childNode = parseCommentNode();
            if (childNode) elementNode->addChild(std::move(childNode));
        } else if (currentToken.type == TokenType::IDENT) {
            if (currentToken.literal == "style") {
                parseStyleBlock(elementNode.get());
            } else if (peekToken.type == TokenType::COLON || peekToken.type == TokenType::EQUAL) {
                parseAttributes(elementNode.get());
            } else {
                auto childNode = parseNode();
                if (childNode) elementNode->addChild(std::move(childNode));
                continue;
            }
        } else {
            errors.push_back("Unexpected token inside element block: " + currentToken.literal);
        }
        nextToken();
    }

    return elementNode;
}

std::unique_ptr<CommentNode> Parser::parseCommentNode() {
    return std::make_unique<CommentNode>(currentToken.literal);
}

} // namespace CHTL
