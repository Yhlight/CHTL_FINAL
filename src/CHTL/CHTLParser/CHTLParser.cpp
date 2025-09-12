#include "CHTLParser.h"
#include <iostream>
#include <stdexcept>

namespace CHTL {

CHTLParser::CHTLParser(const std::vector<Token>& tokens) : tokens_(tokens) {}

std::unique_ptr<RootNode> CHTLParser::parse() {
    auto root = std::make_unique<RootNode>();
    while (!isAtEnd()) {
        try {
            root->children_.push_back(parseDeclaration());
        } catch (const std::runtime_error& e) {
            std::cerr << "Parse Error: " << e.what() << std::endl;
            synchronize();
            hadError_ = true;
        }
    }
    return root;
}

std::unique_ptr<Node> CHTLParser::parseDeclaration() {
    if (match({TokenType::Style})) {
        return parseStyleBlock();
    }
    if (match({TokenType::Identifier})) {
        // It's an element if it's an identifier followed by a brace
        if (peek().type == TokenType::OpenBrace) {
            return parseElement();
        }
    }
    if (match({TokenType::Text})) {
        return parseText();
    }
    if (match({TokenType::GeneratorComment})) {
        return parseGeneratorComment();
    }

    // If no declaration matches, advance and report error.
    advance();
    throw std::runtime_error("Expected a declaration (element, text, etc.).");
}

std::unique_ptr<ElementNode> CHTLParser::parseElement() {
    const Token& name = previous();
    auto element = std::make_unique<ElementNode>(name.lexeme);

    consume(TokenType::OpenBrace, "Expected '{' after element name.");
    parseElementBody(*element);
    consume(TokenType::CloseBrace, "Expected '}' after element body.");

    return element;
}

void CHTLParser::parseElementBody(ElementNode& element) {
    while (!check(TokenType::CloseBrace) && !isAtEnd()) {
        // If it's an identifier followed by a colon, it's an attribute.
        if (peek().type == TokenType::Identifier && peekNext().type == TokenType::Colon) {
            const Token& key = consume(TokenType::Identifier, "Expected attribute name.");
            consume(TokenType::Colon, "Expected ':' after attribute name.");

            std::string value;
            if (match({TokenType::StringLiteral, TokenType::UnquotedLiteral, TokenType::Identifier})) {
                value = previous().lexeme;
            } else {
                throw std::runtime_error("Expected attribute value.");
            }

            consume(TokenType::Semicolon, "Expected ';' after attribute value.");
            element.attributes_.push_back(std::make_unique<AttributeNode>(key.lexeme, value));
        }
        // Otherwise, it must be a declaration (like a child element or style block).
        else {
            element.children_.push_back(parseDeclaration());
        }
    }
}

std::unique_ptr<TextNode> CHTLParser::parseText() {
    consume(TokenType::OpenBrace, "Expected '{' after 'text' keyword.");
    // For now, we only support a single string literal inside text {}
    const Token& content = consume(TokenType::StringLiteral, "Expected a string literal inside text block.");
    consume(TokenType::CloseBrace, "Expected '}' after text content.");
    return std::make_unique<TextNode>(content.lexeme);
}

std::unique_ptr<CommentNode> CHTLParser::parseGeneratorComment() {
    // The lexeme of the GeneratorComment token now holds the content.
    return std::make_unique<CommentNode>(previous().lexeme);
}

std::unique_ptr<StyleBlockNode> CHTLParser::parseStyleBlock() {
    auto styleNode = std::make_unique<StyleBlockNode>();
    consume(TokenType::OpenBrace, "Expected '{' after 'style' keyword.");

    while (!check(TokenType::CloseBrace) && !isAtEnd()) {
        // If it's an identifier followed by a colon, it's an inline property.
        if (peek().type == TokenType::Identifier && peekNext().type == TokenType::Colon) {
            const Token& key = consume(TokenType::Identifier, "Expected CSS property name.");
            consume(TokenType::Colon, "Expected ':' after CSS property name.");

            std::vector<Token> value_tokens;
            while(peek().type != TokenType::Semicolon && !isAtEnd()) {
                value_tokens.push_back(advance());
            }
            if (value_tokens.empty()) {
                throw std::runtime_error("Expected CSS property value.");
            }

            consume(TokenType::Semicolon, "Expected ';' after CSS property value.");
            styleNode->inline_properties_.emplace_back(key.lexeme, value_tokens);
        }
        // Otherwise, assume it's a CSS rule (e.g., .class, #id, &)
        else if (peek().type == TokenType::Identifier || peek().type == TokenType::Ampersand) {
            const Token& selector = consume(TokenType::Identifier, "Expected a CSS selector.");
            auto ruleNode = std::make_unique<CssRuleNode>(selector.lexeme);

            consume(TokenType::OpenBrace, "Expected '{' after selector.");

            while (!check(TokenType::CloseBrace) && !isAtEnd()) {
                const Token& key = consume(TokenType::Identifier, "Expected CSS property name inside rule.");
                consume(TokenType::Colon, "Expected ':' after CSS property name.");

                std::vector<Token> value_tokens;
                while(peek().type != TokenType::Semicolon && !isAtEnd()) {
                    value_tokens.push_back(advance());
                }
                if (value_tokens.empty()) {
                    throw std::runtime_error("Expected CSS property value inside rule.");
                }

                consume(TokenType::Semicolon, "Expected ';' after CSS property value.");
                ruleNode->properties_.emplace_back(key.lexeme, value_tokens);
            }

            consume(TokenType::CloseBrace, "Expected '}' after CSS rule block.");
            styleNode->rules_.push_back(std::move(ruleNode));
        }
        else {
            throw std::runtime_error("Unexpected token inside style block.");
        }
    }

    consume(TokenType::CloseBrace, "Expected '}' after style block.");
    return styleNode;
}

// --- Helper Methods ---

const Token& CHTLParser::peekNext() const {
    if (current_ + 1 >= tokens_.size()) {
        // Return the EOF token if we are at the end
        return tokens_.back();
    }
    return tokens_[current_ + 1];
}

const Token& CHTLParser::peek() const {
    return tokens_[current_];
}

const Token& CHTLParser::previous() const {
    return tokens_[current_ - 1];
}

const Token& CHTLParser::advance() {
    if (!isAtEnd()) current_++;
    return previous();
}

bool CHTLParser::isAtEnd() const {
    return peek().type == TokenType::EndOfFile;
}

bool CHTLParser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool CHTLParser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

const Token& CHTLParser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw std::runtime_error(message + " at line " + std::to_string(peek().line));
}

void CHTLParser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (previous().type == TokenType::Semicolon) return;
        switch (peek().type) {
            case TokenType::Text:
            case TokenType::Style:
            case TokenType::Script:
            case TokenType::Import:
            case TokenType::Namespace:
                return;
            default:
                break;
        }
        advance();
    }
}

} // namespace CHTL
