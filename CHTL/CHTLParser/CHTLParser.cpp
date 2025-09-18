#include "CHTLParser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include <iostream>
#include <stdexcept>

namespace CHTL {

CHTLParser::CHTLParser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::unique_ptr<BaseNode> CHTLParser::parse() {
    // For a CHTL document, we expect a single root element.
    // For now, we will parse one declaration and expect it to be the root.
    if (!isAtEnd() && peek().type != TokenType::END_OF_FILE) {
        return parseDeclaration();
    }
    return nullptr;
}

std::unique_ptr<BaseNode> CHTLParser::parseDeclaration() {
    if (peek().type == TokenType::IDENTIFIER) {
        // Could be an element or a text block.
        if (peek().lexeme == "text") {
            // It's a text block.
            advance(); // consume 'text'
            consume(TokenType::LEFT_BRACE, "Expect '{' after 'text'.");
            Token content = consume(TokenType::STRING, "Expect string literal inside text block.");
            consume(TokenType::RIGHT_BRACE, "Expect '}' after text block content.");
            return std::make_unique<TextNode>(content.lexeme);
        }
        // It's an element.
        return parseElement();
    }

    error(peek(), "Expect a declaration (element or text block).");
    return nullptr; // Should not be reached if error throws
}

std::unique_ptr<ElementNode> CHTLParser::parseElement() {
    Token tagName = consume(TokenType::IDENTIFIER, "Expect element name.");
    auto element = std::make_unique<ElementNode>(tagName.lexeme);

    consume(TokenType::LEFT_BRACE, "Expect '{' after element name.");

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        // Inside the element, we can have attributes or other declarations.
        if (peek().type == TokenType::IDENTIFIER && tokens[current + 1].type == TokenType::COLON) {
            parseAttribute(element.get());
        } else {
            element->addChild(parseDeclaration());
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after element block.");
    return element;
}

void CHTLParser::parseAttribute(ElementNode* element) {
    Token key = consume(TokenType::IDENTIFIER, "Expect attribute name.");
    consume(TokenType::COLON, "Expect ':' after attribute name.");

    Token value;
    if (match({TokenType::STRING, TokenType::IDENTIFIER, TokenType::NUMBER})) {
        value = previous();
    } else {
        error(peek(), "Expect attribute value (string, identifier, or number).");
    }

    consume(TokenType::SEMICOLON, "Expect ';' after attribute value.");

    element->addAttribute({key.lexeme, value.lexeme});
}


// --- Helper Method Implementations ---

bool CHTLParser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

Token CHTLParser::peek() {
    return tokens[current];
}

Token CHTLParser::previous() {
    return tokens[current - 1];
}

Token CHTLParser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool CHTLParser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

Token CHTLParser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    error(peek(), message);
    // This part is tricky because error() might throw. If it doesn't, we need to return something.
    // Let's make error() throw for now to avoid this.
    throw std::runtime_error(message);
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

void CHTLParser::error(const Token& token, const std::string& message) {
    std::cerr << "[line " << token.line << "] Error";
    if (token.type == TokenType::END_OF_FILE) {
        std::cerr << " at end";
    } else {
        std::cerr << " at '" << token.lexeme << "'";
    }
    std::cerr << ": " << message << std::endl;
    // For now, we'll throw an exception to stop parsing.
    // A more sophisticated parser might enter panic mode and try to recover.
    throw std::runtime_error(message);
}

} // namespace CHTL
