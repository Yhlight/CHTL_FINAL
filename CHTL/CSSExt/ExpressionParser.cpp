#include "ExpressionParser.hpp"
#include <stdexcept>

namespace CHTL {
namespace CSSExt {

ExpressionParser::ExpressionParser(const std::vector<Token>& tokens) : m_tokens(tokens) {}

ExprPtr ExpressionParser::parse() {
    try {
        return expression();
    } catch (std::runtime_error& error) {
        // In a real implementation, you'd have better error reporting and recovery.
        return nullptr;
    }
}

// expression -> addition
ExprPtr ExpressionParser::expression() {
    return addition();
}

// addition -> multiplication ( ( "-" | "+" ) multiplication )*
ExprPtr ExpressionParser::addition() {
    ExprPtr expr = multiplication();
    while (match({TokenType::MINUS, TokenType::PLUS})) {
        Token op = previous();
        ExprPtr right = multiplication();
        expr = std::make_shared<Binary>(expr, op, right);
    }
    return expr;
}

// multiplication -> power ( ( "/" | "*" | "%" ) power )*
ExprPtr ExpressionParser::multiplication() {
    ExprPtr expr = power();
    while (match({TokenType::SLASH, TokenType::STAR, TokenType::PERCENT})) {
        Token op = previous();
        ExprPtr right = power();
        expr = std::make_shared<Binary>(expr, op, right);
    }
    return expr;
}

// power -> unary ( "**" unary )*
ExprPtr ExpressionParser::power() {
    ExprPtr expr = unary();
    while (match({TokenType::STAR_STAR})) {
        Token op = previous();
        ExprPtr right = unary();
        expr = std::make_shared<Binary>(expr, op, right);
    }
    return expr;
}

// unary -> ( "-" ) unary | primary
ExprPtr ExpressionParser::unary() {
    if (match({TokenType::MINUS})) {
        Token op = previous();
        ExprPtr right = unary();
        return std::make_shared<Unary>(op, right);
    }
    return primary();
}

// primary -> IDENTIFIER | STRING | "(" expression ")"
ExprPtr ExpressionParser::primary() {
    if (match({TokenType::IDENTIFIER, TokenType::STRING})) {
        return std::make_shared<Literal>(previous());
    }

    if (match({TokenType::LEFT_PAREN})) {
        ExprPtr expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_shared<Grouping>(expr);
    }

    throw std::runtime_error("Expect expression, found token: " + peek().lexeme);
}


// === Helper Methods ===
Token ExpressionParser::advance() {
    if (!isAtEnd()) m_current++;
    return previous();
}

Token ExpressionParser::peek() {
    if (isAtEnd()) {
        // Return a sentinel token that won't match any operators.
        // The last token in a valid list before this would be the final operand.
        if (m_tokens.empty()) return Token{TokenType::END_OF_FILE, "", -1};
        return m_tokens.back();
    }
    return m_tokens[m_current];
}

Token ExpressionParser::previous() {
    return m_tokens[m_current - 1];
}

bool ExpressionParser::isAtEnd() {
    return m_current >= m_tokens.size();
}

bool ExpressionParser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool ExpressionParser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token ExpressionParser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw std::runtime_error(message);
}


} // namespace CSSExt
} // namespace CHTL
