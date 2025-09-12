#include "CHTLParser.h"
#include <stdexcept>

namespace CHTL {

CHTLParser::CHTLParser(std::vector<Token>&& tokens) : tokens(std::move(tokens)) {}

std::unique_ptr<BaseNode> CHTLParser::parse() {
    if (isAtEnd()) return nullptr;
    return parseElement();
}

std::unique_ptr<ElementNode> CHTLParser::parseElement() {
    Token tagToken = consume(TokenType::UNQUOTED_LITERAL, "Expect element tag name.");
    auto element = std::make_unique<ElementNode>(tagToken.lexeme);

    consume(TokenType::LEFT_BRACE, "Expect '{' after element tag name.");
    parseBlock(element.get());
    consume(TokenType::RIGHT_BRACE, "Expect '}' after element block.");

    return element;
}

void CHTLParser::parseBlock(ElementNode* element) {
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        element->addChild(parseElement());
    }
}

// --- Parser Helpers ---

bool CHTLParser::isAtEnd() const {
    return peek().type == TokenType::END_OF_FILE;
}

Token CHTLParser::peek() const {
    if (current >= tokens.size()) return tokens.back(); // EOF
    return tokens[current];
}

Token CHTLParser::advance() {
    if (!isAtEnd()) {
        current++;
    }
    return tokens[current - 1];
}

bool CHTLParser::check(TokenType type) const {
    if (isAtEnd()) return type == TokenType::END_OF_FILE;
    return peek().type == type;
}

Token CHTLParser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw std::runtime_error(message + " Got " + peek().toString());
}

} // namespace CHTL
