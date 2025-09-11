#include "ExpressionParser.h"
#include <stdexcept>

ExpressionParser::ExpressionParser(const std::vector<Token>& tokens) : tokens(tokens) {}

int ExpressionParser::getPrecedence(TokenType type) {
    switch (type) {
        case TokenType::GREATER: case TokenType::LESS: return 10;
        case TokenType::PLUS: case TokenType::MINUS: return 20;
        case TokenType::STAR: case TokenType::SLASH: case TokenType::PERCENT: return 30;
        case TokenType::STAR_STAR: return 40;
        default: return 0;
    }
}

std::unique_ptr<ExprNode> ExpressionParser::parsePrefix() {
    Token token = peek();
    if (token.type == TokenType::NUMBER) {
        Token numToken = advance();
        if (peek().type == TokenType::IDENTIFIER) {
            Token unitToken = advance();
            numToken.lexeme += unitToken.lexeme;
        }
        return std::make_unique<LiteralExprNode>(numToken);
    }
    if (token.type == TokenType::IDENTIFIER) {
        advance();
        return std::make_unique<LiteralExprNode>(token);
    }
    if (token.type == TokenType::LEFT_BRACKET) {
        advance();
        auto expr = parseExpression();
        if (peek().type != TokenType::RIGHT_BRACKET) throw std::runtime_error("Expected ']'");
        advance();
        return expr;
    }
    if (token.type == TokenType::MINUS) {
        advance();
        auto right = parseExpression(4); // High precedence for unary
        return std::make_unique<UnaryOpExprNode>(token, std::move(right));
    }
    return nullptr;
}

std::unique_ptr<ExprNode> ExpressionParser::parseExpression(int precedence) {
    auto left = parsePrefix();
    while (precedence < getPrecedence(peek().type)) {
        Token op = advance();
        int nextPrecedence = getPrecedence(op.type);
        auto right = parseExpression(nextPrecedence);
        left = std::make_unique<BinaryOpExprNode>(std::move(left), op, std::move(right));
    }
    if (peek().type == TokenType::QUESTION) {
        advance(); // consume '?'
        auto thenBranch = parseExpression();
        if (peek().type != TokenType::COLON) throw std::runtime_error("Expected ':'");
        advance(); // consume ':'
        auto elseBranch = parseExpression();
        left = std::make_unique<ConditionalExprNode>(std::move(left), std::move(thenBranch), std::move(elseBranch));
    }
    return left;
}

std::unique_ptr<ExprNode> ExpressionParser::parse() { return parseExpression(); }
Token ExpressionParser::peek() { if (isAtEnd()) return tokens.back(); return tokens[current]; }
Token ExpressionParser::peekNext() { if (current + 1 >= tokens.size()) return tokens.back(); return tokens[current + 1]; }
Token ExpressionParser::advance() { if (!isAtEnd()) current++; return tokens[current - 1]; }
bool ExpressionParser::isAtEnd() { return current >= tokens.size(); }
