#include "Parser.h"
#include "../CHTLNode/StyleNode.h"
#include <iostream>

Parser::Parser(Lexer& lexer) : lexer(lexer) {
    advance();
    advance(); // Initialize both currentToken and nextToken
}

void Parser::advance() {
    currentToken = nextToken;
    nextToken = lexer.getNextToken();
    // Add error handling for lexical errors if needed
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

void Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        advance();
        return;
    }
    // Proper error handling should be implemented here
    std::cerr << "Parse Error: " << message << " at line " << currentToken.line << ". Got " << currentToken.lexeme << std::endl;
    throw std::runtime_error(message);
}

bool Parser::check(TokenType type) {
    return currentToken.type == type;
}

bool Parser::checkNext(TokenType type) {
    return nextToken.type == type;
}

std::unique_ptr<ElementNode> Parser::parse() {
    auto root = std::make_unique<ElementNode>();
    root->tagName = "root";
    while (!check(TokenType::END_OF_FILE)) {
        auto decl = declaration();
        if (decl) {
            root->children.push_back(std::move(decl));
        }
    }
    return root;
}

std::unique_ptr<BaseNode> Parser::declaration() {
    if (check(TokenType::IDENTIFIER)) {
        if (checkNext(TokenType::LEFT_BRACE)) {
            return element();
        }
    }
    if (check(TokenType::TEXT)) {
        return textNode();
    }
    if (check(TokenType::STYLE)) {
        return styleNode();
    }

    std::cerr << "Parse Error: Unexpected token " << currentToken.lexeme << " at line " << currentToken.line << std::endl;
    advance();
    return nullptr;
}

std::unique_ptr<ElementNode> Parser::element() {
    auto node = std::make_unique<ElementNode>();
    node->tagName = currentToken.lexeme;
    advance(); // consume identifier

    consume(TokenType::LEFT_BRACE, "Expect '{' after element name.");

    while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::END_OF_FILE)) {
        if (check(TokenType::IDENTIFIER)) {
            if (checkNext(TokenType::COLON) || checkNext(TokenType::EQUAL)) {
                attributes(*node);
            } else if (checkNext(TokenType::LEFT_BRACE)) {
                auto child = element();
                if (child) {
                    node->children.push_back(std::move(child));
                }
            } else {
                std::cerr << "Parse Error: Unexpected token " << nextToken.lexeme << " after identifier " << currentToken.lexeme << " at line " << currentToken.line << std::endl;
                advance();
            }
        } else if (check(TokenType::TEXT)) {
            auto child = textNode();
            if (child) {
                node->children.push_back(std::move(child));
            }
        } else if (check(TokenType::STYLE)) {
            auto child = styleNode();
            if (child) {
                node->children.push_back(std::move(child));
            }
        } else {
            std::cerr << "Parse Error: Unexpected token " << currentToken.lexeme << " in element " << node->tagName << " at line " << currentToken.line << std::endl;
            advance();
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after element block.");

    return node;
}

std::unique_ptr<TextNode> Parser::textNode() {
    consume(TokenType::TEXT, "Expect 'text' keyword.");
    consume(TokenType::LEFT_BRACE, "Expect '{' after 'text'.");
    auto node = std::make_unique<TextNode>();
    if (check(TokenType::STRING) || check(TokenType::IDENTIFIER)) {
        node->content = currentToken.lexeme;
        advance();
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after text content.");
    return node;
}

std::unique_ptr<StyleNode> Parser::styleNode() {
    consume(TokenType::STYLE, "Expect 'style' keyword.");
    consume(TokenType::LEFT_BRACE, "Expect '{' after 'style'.");

    auto node = std::make_unique<StyleNode>();

    while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::END_OF_FILE)) {
        if (check(TokenType::IDENTIFIER)) {
            std::string key = currentToken.lexeme;
            advance(); // consume property name

            consume(TokenType::COLON, "Expect ':' after style property name.");

            if (check(TokenType::STRING) || check(TokenType::IDENTIFIER)) {
                node->properties[key] = currentToken.lexeme;
                advance(); // consume property value
            } else {
                std::cerr << "Parse Error: Expected style property value at line " << currentToken.line << std::endl;
                // Allow for recovery by advancing
                advance();
            }

            consume(TokenType::SEMICOLON, "Expect ';' after style property value.");
        } else {
            std::cerr << "Parse Error: Unexpected token " << currentToken.lexeme << " in style block at line " << currentToken.line << std::endl;
            advance(); // Advance to avoid infinite loops
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after style block.");
    return node;
}

void Parser::attributes(ElementNode& element) {
    while (check(TokenType::IDENTIFIER) && (checkNext(TokenType::COLON) || checkNext(TokenType::EQUAL))) {
        std::string key = currentToken.lexeme;
        advance(); // consume key

        if (match(TokenType::COLON) || match(TokenType::EQUAL)) {
            if (check(TokenType::STRING) || check(TokenType::IDENTIFIER)) {
                element.attributes[key] = currentToken.lexeme;
                advance();
            } else {
                 std::cerr << "Parse Error: Expected attribute value at line " << currentToken.line << std::endl;
            }
            consume(TokenType::SEMICOLON, "Expect ';' after attribute value.");
        }
    }
}
