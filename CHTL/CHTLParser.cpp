#include "CHTLParser.h"
#include <vector>

namespace CHTL {

Parser::Parser(const std::vector<Token>& tokens) : tokens_(tokens) {}

NodeList Parser::parse() {
    NodeList statements;
    while (!isAtEnd()) {
        statements.push_back(declaration());
    }
    return statements;
}

NodePtr Parser::declaration() {
    if (match({TokenType::IDENTIFIER})) {
        return elementDeclaration();
    }
    if (match({TokenType::KEYWORD_TEXT})) {
        return textDeclaration();
    }
    // For now, we'll skip over things we don't understand, like attributes.
    // A more robust parser would handle these or error out.
    // We also need to handle comments that made it through the lexer.
    if (match({TokenType::SEMICOLON, TokenType::COMMENT})) {
        // In a real parser, we might create a comment node or just skip.
        // For now, returning nullptr and the main loop will advance.
        // A better approach would be to loop here until we find a real declaration.
        return nullptr;
    }


    throw ParseError("Expect a declaration (element or text). Found token: " + peek().lexeme);
}

NodePtr Parser::elementDeclaration() {
    const Token& name = previous();
    auto node = std::make_shared<ElementNode>(name.lexeme);

    consume(TokenType::LEFT_BRACE, "Expect '{' after element name.");

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        NodePtr child = declaration();
        if (child) { // The simple declaration() can return nullptr for comments/semicolons
            node->children.push_back(child);
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after element block.");
    return node;
}

NodePtr Parser::textDeclaration() {
    consume(TokenType::LEFT_BRACE, "Expect '{' after 'text' keyword.");
    const Token& content = consume(TokenType::STRING, "Expect string literal inside text block.");
    consume(TokenType::RIGHT_BRACE, "Expect '}' after text block.");
    return std::make_shared<TextNode>(content.lexeme);
}


// --- Helper Methods ---

bool Parser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

const Token& Parser::peek() {
    return tokens_[current_];
}

const Token& Parser::previous() {
    return tokens_[current_ - 1];
}

const Token& Parser::advance() {
    if (!isAtEnd()) current_++;
    return previous();
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

const Token& Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw ParseError(message);
}

} // namespace CHTL
