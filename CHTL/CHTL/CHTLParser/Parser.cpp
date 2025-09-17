#include "Parser.h"
#include <iostream>
#include <stdexcept>

// Helper function to unwrap quoted strings
static std::string unwrapString(const std::string& s) {
    if (s.length() >= 2 && (s.front() == '"' || s.front() == '\'') && s.front() == s.back()) {
        return s.substr(1, s.length() - 2);
    }
    return s;
}

Parser::Parser(Lexer& lexer) {
    // Fill the token buffer
    for (Token token = lexer.nextToken(); token.type != TokenType::TOKEN_EOF; token = lexer.nextToken()) {
        if (token.type != TokenType::TOKEN_ERROR) {
            tokens.push_back(token);
        } else {
            std::cerr << "[Line " << token.line << "] Lexer Error: " << token.value << std::endl;
        }
    }
    tokens.push_back({TokenType::TOKEN_EOF, "", tokens.empty() ? 1 : tokens.back().line});
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

Token Parser::peek() {
    return tokens[current];
}

Token Parser::previous() {
    return tokens[current - 1];
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::TOKEN_EOF;
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
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
    throw std::runtime_error(message);
}

void Parser::reportError(const std::string& message) {
    std::cerr << "[Line " << (current > 0 ? previous().line : peek().line) << "] Parse Error: " << message << std::endl;
}

void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (previous().type == TokenType::TOKEN_SEMICOLON) return;
        switch (peek().type) {
            case TokenType::TOKEN_IDENTIFIER:
                return;
            default:
                break;
        }
        advance();
    }
}

std::shared_ptr<RootNode> Parser::parse() {
    auto root = std::make_shared<RootNode>();
    while (!isAtEnd()) {
        try {
            root->statements.push_back(parseDeclaration());
        } catch (const std::runtime_error& e) {
            reportError(e.what());
            synchronize();
        }
    }
    return root;
}

std::shared_ptr<Node> Parser::parseDeclaration() {
    if (check(TokenType::TOKEN_IDENTIFIER)) {
        // A declaration must be an element or a text block.
        // Since text blocks can only exist inside elements, any top-level
        // declaration must be an element.
        return parseElement();
    }
    throw std::runtime_error("Expected a top-level element declaration.");
}

std::shared_ptr<AttributeNode> Parser::parseAttribute() {
    auto attr = std::make_shared<AttributeNode>();
    attr->name = peek().value;
    advance(); // Consume identifier

    if (!match(TokenType::TOKEN_COLON) && !match(TokenType::TOKEN_EQUALS)) {
        throw std::runtime_error("Expect ':' or '=' after attribute name.");
    }

    // An attribute value can be a string or another identifier (unquoted literal)
    if (check(TokenType::TOKEN_STRING_LITERAL) || check(TokenType::TOKEN_IDENTIFIER)) {
        attr->value = unwrapString(peek().value);
        advance();
    } else {
        throw std::runtime_error("Expect attribute value (string or unquoted literal).");
    }

    consume(TokenType::TOKEN_SEMICOLON, "Expect ';' after attribute value.");
    return attr;
}

std::shared_ptr<TextNode> Parser::parseText() {
    consume(TokenType::TOKEN_IDENTIFIER, "Expect 'text' keyword."); // Consume 'text'
    auto textNode = std::make_shared<TextNode>();

    if (match(TokenType::TOKEN_LEFT_BRACE)) {
        if (check(TokenType::TOKEN_STRING_LITERAL) || check(TokenType::TOKEN_IDENTIFIER)) {
            textNode->content = unwrapString(peek().value);
            advance();
        }
        consume(TokenType::TOKEN_RIGHT_BRACE, "Expect '}' after text block.");
    } else if (match(TokenType::TOKEN_COLON)) {
         if (check(TokenType::TOKEN_STRING_LITERAL) || check(TokenType::TOKEN_IDENTIFIER)) {
            textNode->content = unwrapString(peek().value);
            advance();
        }
        consume(TokenType::TOKEN_SEMICOLON, "Expect ';' after text value.");
    } else {
        throw std::runtime_error("Expect ':' or '{' after 'text' keyword.");
    }

    return textNode;
}

std::shared_ptr<ElementNode> Parser::parseElement() {
    auto element = std::make_shared<ElementNode>();
    element->tagName = peek().value;
    advance(); // Consume tag name identifier

    consume(TokenType::TOKEN_LEFT_BRACE, "Expect '{' after element name.");

    while (!check(TokenType::TOKEN_RIGHT_BRACE) && !isAtEnd()) {
        if (peek().type != TokenType::TOKEN_IDENTIFIER) {
            throw std::runtime_error("Unexpected token in element body. Expected attribute, nested element, or 'text'.");
        }

        if (peek().value == "text") {
            // If the keyword is 'text', it's always a text node.
            element->children.push_back(parseText());
        } else {
            // It's another identifier. Look ahead to see if it's an attribute or nested element.
            Token nextToken = tokens[current + 1];

            if (nextToken.type == TokenType::TOKEN_COLON || nextToken.type == TokenType::TOKEN_EQUALS) {
                element->attributes.push_back(parseAttribute());
            } else if (nextToken.type == TokenType::TOKEN_LEFT_BRACE) {
                element->children.push_back(parseElement());
            } else {
                 throw std::runtime_error("Invalid statement inside element. Expected attribute (ident:val;) or nested element (ident{...}).");
            }
        }
    }

    consume(TokenType::TOKEN_RIGHT_BRACE, "Expect '}' to close element block.");
    return element;
}
