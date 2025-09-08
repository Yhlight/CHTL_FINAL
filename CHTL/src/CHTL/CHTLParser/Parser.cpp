#include "Parser.h"
#include <stdexcept>

namespace CHTL {

Parser::Parser(const std::vector<Token>& tokens) : m_tokens(tokens) {}

std::unique_ptr<DocumentNode> Parser::parse() {
    auto document = std::make_unique<DocumentNode>();
    while (!isAtEnd()) {
        document->addChild(parseNode());
    }
    return document;
}

std::unique_ptr<BaseNode> Parser::parseNode() {
    if (peek().type == TokenType::Identifier) {
        if (peek().value == "text") {
            return parseTextElement();
        }
        return parseElement();
    }
    throw std::runtime_error("Unexpected token while parsing a node: " + peek().value);
}

std::unique_ptr<ElementNode> Parser::parseElement() {
    const Token& tagNameToken = consume(TokenType::Identifier, "Expected element tag name.");
    auto element = std::make_unique<ElementNode>(tagNameToken.value);

    consume(TokenType::OpenBrace, "Expected '{' after element tag name.");

    while (!check(TokenType::CloseBrace) && !isAtEnd()) {
        if (m_current + 1 < m_tokens.size() && (m_tokens[m_current + 1].type == TokenType::Colon || m_tokens[m_current + 1].type == TokenType::Equals)) {
            parseAttributes(element.get());
        } else {
            element->addChild(parseNode());
        }
    }

    consume(TokenType::CloseBrace, "Expected '}' to close element.");
    return element;
}

std::unique_ptr<TextNode> Parser::parseTextElement() {
    consume(TokenType::Identifier, "Expected 'text' keyword.");
    consume(TokenType::OpenBrace, "Expected '{' after 'text' keyword.");

    const Token& content = advance();
    if (content.type != TokenType::StringLiteral && content.type != TokenType::Identifier) {
        throw std::runtime_error("Expected string or unquoted literal inside text block.");
    }

    auto textNode = std::make_unique<TextNode>(content.value);

    consume(TokenType::CloseBrace, "Expected '}' to close text block.");
    return textNode;
}

void Parser::parseAttributes(ElementNode* element) {
    const Token& keyToken = consume(TokenType::Identifier, "Expected attribute key.");

    if (!match(TokenType::Colon) && !match(TokenType::Equals)) {
        throw std::runtime_error("Expected ':' or '=' after attribute key.");
    }

    const Token& valueToken = advance();
    if (valueToken.type != TokenType::StringLiteral && valueToken.type != TokenType::Identifier) {
        throw std::runtime_error("Expected attribute value.");
    }

    consume(TokenType::Semicolon, "Expected ';' after attribute value.");

    element->addAttribute(std::make_unique<AttributeNode>(keyToken.value, valueToken.value));
}

// --- Token utility functions ---

const Token& Parser::peek() const {
    return m_tokens[m_current];
}

const Token& Parser::advance() {
    if (!isAtEnd()) m_current++;
    return m_tokens[m_current - 1];
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::EndOfFile;
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

const Token& Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    std::string error_message = message + " Found token of type " + std::to_string((int)peek().type) + " with value '" + peek().value + "' instead.";
    throw std::runtime_error(error_message);
}

} // namespace CHTL
