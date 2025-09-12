#include "ExpressionParser.h"
#include <stdexcept>

namespace CHTL
{
    ExpressionParser::ExpressionParser(std::vector<Token> tokens)
        : m_tokens(std::move(tokens)), m_position(0) {}

    Token& ExpressionParser::currentToken() {
        if (m_position >= m_tokens.size()) {
            // Should not happen if grammar is correct, return last token (likely EOF or delimiter)
            return m_tokens.back();
        }
        return m_tokens[m_position];
    }

    void ExpressionParser::advance() {
        if (m_position < m_tokens.size()) {
            m_position++;
        }
    }

    std::unique_ptr<ExprNode> ExpressionParser::parse() {
        auto expr = parseAddition();
        if (m_position < m_tokens.size()) {
            throw std::runtime_error("Unexpected tokens at end of expression: " + currentToken().literal);
        }
        return expr;
    }

    // Handles Addition and Subtraction (lowest precedence)
    std::unique_ptr<ExprNode> ExpressionParser::parseAddition() {
        auto left = parseMultiplication();

        while (currentToken().type == TokenType::PLUS || currentToken().type == TokenType::MINUS) {
            char op = currentToken().literal[0];
            advance();
            auto right = parseMultiplication();
            left = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
        }

        return left;
    }

    // Handles Multiplication, Division, Modulo
    std::unique_ptr<ExprNode> ExpressionParser::parseMultiplication() {
        auto left = parsePower();

        while (currentToken().type == TokenType::STAR || currentToken().type == TokenType::SLASH || currentToken().type == TokenType::PERCENT) {
            char op = currentToken().literal[0];
            advance();
            auto right = parsePower();
            left = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
        }

        return left;
    }

    // Handles Power (right-associative, but we'll do left-associative for simplicity for now)
    std::unique_ptr<ExprNode> ExpressionParser::parsePower() {
        auto left = parsePrimary();

        while (currentToken().type == TokenType::POWER) {
            char op = '^'; // Using ^ as a symbol for power op
            advance();
            auto right = parsePrimary();
            left = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
        }

        return left;
    }

    // Handles numbers, strings, identifiers, references, and parentheses (highest precedence)
    std::unique_ptr<ExprNode> ExpressionParser::parsePrimary() {
        // Handle parentheses first
        if (currentToken().type == TokenType::LEFT_PAREN) {
            advance(); // consume '('
            auto expr = parse();
            if (currentToken().type != TokenType::RIGHT_PAREN) {
                throw std::runtime_error("Mismatched parentheses in expression.");
            }
            advance(); // consume ')'
            return expr;
        }

        // Handle numbers
        if (currentToken().type == TokenType::NUMBER) {
            double value = std::stod(currentToken().literal);
            advance();
            std::string unit = "";
            if (m_position < m_tokens.size() && currentToken().type == TokenType::IDENTIFIER) {
                unit = currentToken().literal;
                advance();
            }
            return std::make_unique<NumberLiteralNode>(value, unit);
        }

        // Handle references (e.g., .box.width or box.width)
        std::string selector;
        if (currentToken().type == TokenType::DOT) {
            selector += currentToken().literal;
            advance();
            if (currentToken().type != TokenType::IDENTIFIER) throw std::runtime_error("Expected identifier after '.' in selector.");
            selector += currentToken().literal;
            advance();
        } else if (currentToken().type == TokenType::IDENTIFIER) {
             // It could be a reference or a standalone identifier. Look ahead.
             if (m_position + 1 < m_tokens.size() && m_tokens[m_position + 1].type == TokenType::DOT) {
                selector = currentToken().literal;
                advance(); // consume selector
             } else {
                // This is an arithmetic expression parser, a standalone identifier is not valid.
                throw std::runtime_error("Unexpected identifier '" + currentToken().literal + "' in arithmetic expression.");
             }
        }

        if (!selector.empty()) { // We have started parsing a reference
            if (currentToken().type != TokenType::DOT) throw std::runtime_error("Expected '.' after selector in attribute reference.");
            advance(); // consume dot
            if (currentToken().type != TokenType::IDENTIFIER) throw std::runtime_error("Expected property name after dot.");
            std::string propertyName = currentToken().literal;
            advance();
            return std::make_unique<AttributeReferenceNode>(selector, propertyName);
        }

        throw std::runtime_error("Unexpected token in expression: " + currentToken().literal);
    }
}
