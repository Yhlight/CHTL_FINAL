#include "Parser.h"
#include <stdexcept>
#include <iostream>

namespace CHTL {

Parser::Parser(const std::vector<Token>& tokens) : m_tokens(tokens) {}

// --- Token Management ---

bool Parser::isAtEnd() const {
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::peek() const {
    return m_tokens[m_current];
}

Token Parser::peekNext() const {
    if (isAtEnd() || m_current + 1 >= m_tokens.size()) return peek();
    return m_tokens[m_current + 1];
}

Token Parser::advance() {
    if (!isAtEnd()) m_current++;
    return m_tokens[m_current - 1];
}

bool Parser::check(TokenType type) const {
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

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        return advance();
    }
    throw std::runtime_error("Parse Error: " + message + " at line " + std::to_string(peek().line) + ". Got " + peek().lexeme);
}

// --- Parsing Logic ---

DocumentNode Parser::parse() {
    DocumentNode statements;
    while (!isAtEnd()) {
        auto statement = parseStatement();
        if (statement) {
            statements.push_back(std::move(statement));
        }
    }
    return statements;
}

std::unique_ptr<BaseNode> Parser::parseStatement() {
    if (check(TokenType::KEYWORD_TEXT)) {
        return parseText();
    }
    if (check(TokenType::IDENTIFIER)) {
        return parseElement();
    }

    std::cerr << "Warning: Skipping unexpected token '" << peek().lexeme
              << "' at line " << std::to_string(peek().line) << std::endl;
    advance();
    return nullptr;
}

AttributeNode Parser::parseAttribute() {
    AttributeNode attr;
    attr.key = consume(TokenType::IDENTIFIER, "Expect attribute key.").lexeme;

    if (!match(TokenType::COLON) && !match(TokenType::EQUAL)) {
        throw std::runtime_error("Parse Error: Expect ':' or '=' after attribute key '" + attr.key + "'.");
    }

    if (check(TokenType::STRING_LITERAL) || check(TokenType::IDENTIFIER)) {
        attr.value = advance().lexeme;
    } else {
        throw std::runtime_error("Parse Error: Expected attribute value for key '" + attr.key + "'.");
    }

    match(TokenType::SEMICOLON);
    return attr;
}

std::unique_ptr<TextNode> Parser::parseText() {
    consume(TokenType::KEYWORD_TEXT, "Expect 'text' keyword.");
    consume(TokenType::LBRACE, "Expect '{' after 'text'.");

    auto textNode = std::make_unique<TextNode>();
    if (check(TokenType::STRING_LITERAL)) {
        textNode->content = advance().lexeme;
    }

    consume(TokenType::RBRACE, "Expect '}' after text block content.");
    return textNode;
}

std::unique_ptr<ElementNode> Parser::parseElement() {
    auto element = std::make_unique<ElementNode>();
    element->tagName = consume(TokenType::IDENTIFIER, "Expect element tag name.").lexeme;

    consume(TokenType::LBRACE, "Expect '{' after element tag name.");

    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        if (check(TokenType::IDENTIFIER) && (peekNext().type == TokenType::COLON || peekNext().type == TokenType::EQUAL)) {
            element->attributes.push_back(parseAttribute());
        }
        else {
            auto child = parseStatement();
            if (child) {
                element->children.push_back(std::move(child));
            }
        }
    }

    consume(TokenType::RBRACE, "Expect '}' after element block.");
    return element;
}

}
