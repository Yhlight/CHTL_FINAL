#include "ExpressionParser.h"

ExpressionParser::ExpressionParser(const std::vector<Token>& tokens) : tokens(tokens) {}

// --- Pratt Parser Implementation ---

int ExpressionParser::getPrecedence(TokenType type) {
    switch (type) {
        case TokenType::PLUS:
        case TokenType::MINUS:
            return 1;
        case TokenType::STAR:
        case TokenType::SLASH:
        case TokenType::PERCENT:
            return 2;
        case TokenType::STAR_STAR:
            return 3; // Right-associative, but we'll handle that in the loop
        default:
            return 0; // Not an infix operator
    }
}

std::unique_ptr<ExprNode> ExpressionParser::parsePrefix() {
    Token token = peek();
    if (token.type == TokenType::NUMBER) {
        Token numToken = advance();
        if (peek().type == TokenType::IDENTIFIER) {
            // It's a number with a unit, e.g., 100px.
            // Combine them into a single literal token for the evaluator.
            Token unitToken = advance();
            numToken.lexeme += unitToken.lexeme;
        }
        return std::make_unique<LiteralExprNode>(numToken);
    }

    if (token.type == TokenType::IDENTIFIER) {
        advance();
        return std::make_unique<LiteralExprNode>(token);
    }

    if (token.type == TokenType::LEFT_BRACKET) { // Assuming [ is used for grouping, as ( might be for function calls later
        advance(); // consume [
        auto expr = parseExpression();
        if (peek().type != TokenType::RIGHT_BRACKET) {
            // Error
        }
        advance(); // consume ]
        return expr;
    }

    if (token.type == TokenType::MINUS) {
        advance();
        auto right = parseExpression(4); // Use a high precedence for unary operators
        return std::make_unique<UnaryOpExprNode>(token, std::move(right));
    }

    return nullptr; // Or throw an error for unexpected token
}

std::unique_ptr<ExprNode> ExpressionParser::parseExpression(int precedence) {
    std::unique_ptr<ExprNode> left = parsePrefix();

    while (precedence < getPrecedence(peek().type)) {
        Token op = advance();
        int nextPrecedence = getPrecedence(op.type);
        // For right-associative operators like power (**), we'd use `<` instead of `<=`
        // For simplicity, we'll treat all as left-associative for now.
        std::unique_ptr<ExprNode> right = parseExpression(nextPrecedence);
        left = std::make_unique<BinaryOpExprNode>(std::move(left), op, std::move(right));
    }

    return left;
}

std::unique_ptr<ExprNode> ExpressionParser::parse() {
    return parseExpression();
}


// --- Helper Methods ---

Token ExpressionParser::peek() {
    if (isAtEnd()) return tokens.back(); // Should be EOF
    return tokens[current];
}

Token ExpressionParser::advance() {
    if (!isAtEnd()) current++;
    return tokens[current - 1];
}

bool ExpressionParser::isAtEnd() {
    // The expression parser receives a slice of tokens, so EOF is not guaranteed.
    // We are at the end if the current index is out of bounds.
    return current >= tokens.size();
}
