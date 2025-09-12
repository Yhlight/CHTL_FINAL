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
        // Check for attributes first
        if (peek().type == TokenType::Identifier && peekNext().type == TokenType::Colon) {
            parseAttributes(element);
        } else {
            element.children_.push_back(parseDeclaration());
        }
    }
}

void CHTLParser::parseAttributes(ElementNode& element) {
    while (peek().type == TokenType::Identifier && peekNext().type == TokenType::Colon) {
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
        const Token& key = consume(TokenType::Identifier, "Expected CSS property name.");
        consume(TokenType::Colon, "Expected ':' after CSS property name.");

        // For now, we assume the value is a single unquoted literal token.
        // This will need to be made more robust for multi-token values.
        std::string value;
        if (match({TokenType::UnquotedLiteral, TokenType::Identifier, TokenType::StringLiteral})) {
            value = previous().lexeme;
        } else {
            throw std::runtime_error("Expected CSS property value.");
        }

        consume(TokenType::Semicolon, "Expected ';' after CSS property value.");
        styleNode->properties_.emplace_back(key.lexeme, value);
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
