#include "CssExpressionEvaluator.h"
#include "CssValue.h"
#include <vector>
#include <stack>
#include <stdexcept>
#include <sstream>
#include <cmath> // For std::pow

namespace CHTL {

// Helper to get operator precedence
int CssExpressionEvaluator::getPrecedence(TokenType type) {
    switch (type) {
        case TokenType::Plus:
        case TokenType::Minus:
            return 1;
        case TokenType::Asterisk:
        case TokenType::Slash:
        case TokenType::Percent:
            return 2;
        case TokenType::DoubleAsterisk:
            return 3;
        default:
            return 0; // Not an operator
    }
}

std::string CssExpressionEvaluator::evaluate(const std::vector<Token>& tokens) {
    if (tokens.empty()) {
        return "";
    }

    // Fallback for non-calculable expressions
    auto fallback = [&]() {
        std::stringstream ss;
        for (size_t i = 0; i < tokens.size(); ++i) {
            ss << tokens[i].lexeme << (i == tokens.size() - 1 ? "" : " ");
        }
        return ss.str();
    };

    // Pre-processing step: Combine numbers with their units and disambiguate '%'
    std::vector<Token> processedTokens;
    for (size_t i = 0; i < tokens.size(); ++i) {
        // Look for a number followed by a unit (Identifier or Percent)
        if (tokens[i].type == TokenType::Number && i + 1 < tokens.size()) {
            const auto& nextToken = tokens[i+1];
            bool isUnit = false;

            if (nextToken.type == TokenType::Identifier) {
                isUnit = true; // Identifiers after numbers are always units
            } else if (nextToken.type == TokenType::Percent) {
                // '%' is a unit if it's at the end, or followed by an operator/paren
                if (i + 2 >= tokens.size() || getPrecedence(tokens[i+2].type) > 0 || tokens[i+2].type == TokenType::CloseParen) {
                    isUnit = true;
                }
            }

            if (isUnit) {
                Token combinedToken = tokens[i];
                combinedToken.lexeme += nextToken.lexeme;
                processedTokens.push_back(combinedToken);
                i++; // Skip the unit token
            } else {
                processedTokens.push_back(tokens[i]); // It was a number, but not followed by a unit
            }
        } else {
            processedTokens.push_back(tokens[i]); // Not a number, or last token
        }
    }

    // Shunting-yard algorithm
    std::vector<Token> outputQueue;
    std::stack<Token> operatorStack;

    for (const auto& token : processedTokens) {
        if (token.type == TokenType::Number || token.type == TokenType::UnquotedLiteral || token.type == TokenType::Identifier) {
            outputQueue.push_back(token);
        } else if (getPrecedence(token.type) > 0) { // Is an operator
            while (!operatorStack.empty() && getPrecedence(operatorStack.top().type) >= getPrecedence(token.type)) {
                outputQueue.push_back(operatorStack.top());
                operatorStack.pop();
            }
            operatorStack.push(token);
        } else if (token.type == TokenType::OpenParen) {
            operatorStack.push(token);
        } else if (token.type == TokenType::CloseParen) {
            while (!operatorStack.empty() && operatorStack.top().type != TokenType::OpenParen) {
                outputQueue.push_back(operatorStack.top());
                operatorStack.pop();
            }
            if (operatorStack.empty()) return fallback(); // Mismatched parentheses
            operatorStack.pop(); // Pop '('
        } else {
            // If we encounter a token that is not a value, operator, or parenthesis,
            // it's part of an expression we cannot calculate.
            return fallback();
        }
    }

    while (!operatorStack.empty()) {
        if (operatorStack.top().type == TokenType::OpenParen) return fallback(); // Mismatched parentheses
        outputQueue.push_back(operatorStack.top());
        operatorStack.pop();
    }

    // RPN evaluation
    std::stack<CssValue> valueStack;
    for (const auto& token : outputQueue) {
        if (token.type == TokenType::Number || token.type == TokenType::UnquotedLiteral || token.type == TokenType::Identifier) {
            try {
                 valueStack.push(parseCssValue(token.lexeme));
            } catch (const std::invalid_argument&) {
                return fallback(); // Cannot parse value
            }
        } else { // Is an operator
            if (valueStack.size() < 2) return fallback(); // Not enough operands
            CssValue right = valueStack.top(); valueStack.pop();
            CssValue left = valueStack.top(); valueStack.pop();
            CssValue result;

            // Unit compatibility check
            if (left.unit != right.unit && !left.unit.empty() && !right.unit.empty()) {
                 return fallback(); // Incompatible units
            }
            result.unit = left.unit.empty() ? right.unit : left.unit;

            switch (token.type) {
                case TokenType::Plus:
                    result.value = left.value + right.value;
                    break;
                case TokenType::Minus:
                    result.value = left.value - right.value;
                    break;
                case TokenType::Asterisk:
                    result.value = left.value * right.value;
                     // If one is unitless, the result takes the unit of the other
                    if (left.unit.empty() || right.unit.empty()) {
                       // Unit is already set correctly
                    } else {
                        // This case (e.g., px * px) is invalid in CSS
                        return fallback();
                    }
                    break;
                case TokenType::Slash:
                    if (right.value == 0) return fallback(); // Division by zero
                    result.value = left.value / right.value;
                    // If one is unitless, the result takes the unit of the other
                    if (left.unit.empty() || right.unit.empty()) {
                       // Unit is already set correctly
                    } else {
                       // px / px should result in a unitless number
                       result.unit = "";
                    }
                    break;
                case TokenType::Percent: // Modulo
                    result.value = fmod(left.value, right.value);
                    break;
                case TokenType::DoubleAsterisk: // Power
                    result.value = std::pow(left.value, right.value);
                     if (!right.unit.empty()) {
                        // Raising to a power with a unit is invalid
                        return fallback();
                    }
                    break;
                default:
                    return fallback(); // Should not happen
            }
            valueStack.push(result);
        }
    }

    if (valueStack.size() != 1) {
        return fallback(); // Invalid expression
    }

    std::stringstream result_ss;
    result_ss << valueStack.top().value << valueStack.top().unit;
    return result_ss.str();
}

} // namespace CHTL
