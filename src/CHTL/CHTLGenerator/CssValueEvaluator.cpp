#include "CssValueEvaluator.h"
#include <stdexcept>
#include <cmath>
#include <stack>
#include <iostream>
#include <algorithm>
#include <sstream>

namespace CHTL {

// Main public method
std::string CssValueEvaluator::evaluate(const std::vector<Token>& tokens) {
    try {
        auto rpn = toRPN(tokens);
        return evaluateRPN(rpn);
    } catch (const std::exception& e) {
        // If evaluation fails, fall back to original calc() representation.
        // This makes the feature robust to parsing errors or complex cases.
        std::cerr << "CSS evaluation error: " << e.what() << ". Falling back to calc()." << std::endl;
        std::stringstream ss;
        ss << "calc(";
        for (size_t i = 0; i < tokens.size(); ++i) {
            ss << tokens[i].lexeme;
            if (i < tokens.size() - 1) {
                ss << " ";
            }
        }
        ss << ")";
        return ss.str();
    }
}

// Parses a string like "10px" into a {10, "px"} struct
CssValueEvaluator::ValueUnit CssValueEvaluator::parseValueUnit(const std::string& lexeme) {
    size_t first_alpha = lexeme.find_first_not_of("-.0123456789");
    if (first_alpha == std::string::npos) {
        return {std::stod(lexeme), ""};
    }
    std::string num_part = lexeme.substr(0, first_alpha);
    std::string unit_part = lexeme.substr(first_alpha);
    return {std::stod(num_part), unit_part};
}

// Defines operator precedence
int CssValueEvaluator::getPrecedence(TokenType type) {
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
            return 0;
    }
}

// Converts infix token stream to Reverse Polish Notation (RPN) using Shunting-yard algorithm
std::vector<CssValueEvaluator::RpnToken> CssValueEvaluator::toRPN(const std::vector<Token>& tokens) {
    std::vector<RpnToken> output_queue;
    std::stack<Token> operator_stack;

    for (const auto& token : tokens) {
        switch (token.type) {
            case TokenType::Number:
                output_queue.push_back(parseValueUnit(token.lexeme));
                break;

            case TokenType::Plus:
            case TokenType::Minus:
            case TokenType::Asterisk:
            case TokenType::Slash:
            case TokenType::Percent:
            case TokenType::DoubleAsterisk:
                while (!operator_stack.empty() &&
                       getPrecedence(operator_stack.top().type) >= getPrecedence(token.type)) {
                    output_queue.push_back(operator_stack.top().type);
                    operator_stack.pop();
                }
                operator_stack.push(token);
                break;

            case TokenType::OpenParen:
                operator_stack.push(token);
                break;

            case TokenType::CloseParen:
                while (!operator_stack.empty() && operator_stack.top().type != TokenType::OpenParen) {
                    output_queue.push_back(operator_stack.top().type);
                    operator_stack.pop();
                }
                if (operator_stack.empty()) {
                    throw std::runtime_error("Mismatched parentheses in expression.");
                }
                operator_stack.pop(); // Pop the '('
                break;

            default:
                // Ignore other tokens for now, or throw an error for unexpected tokens in an expression
                break;
        }
    }

    while (!operator_stack.empty()) {
        if (operator_stack.top().type == TokenType::OpenParen) {
            throw std::runtime_error("Mismatched parentheses in expression.");
        }
        output_queue.push_back(operator_stack.top().type);
        operator_stack.pop();
    }

    return output_queue;
}

// Evaluates the RPN queue to a final string value
std::string CssValueEvaluator::evaluateRPN(std::vector<RpnToken>& rpn) {
    std::stack<ValueUnit> value_stack;

    for (const auto& rpn_token : rpn) {
        if (std::holds_alternative<ValueUnit>(rpn_token)) {
            value_stack.push(std::get<ValueUnit>(rpn_token));
        } else {
            TokenType op = std::get<TokenType>(rpn_token);

            if (value_stack.size() < 2) {
                throw std::runtime_error("Invalid expression: not enough operands for operator.");
            }
            ValueUnit rhs = value_stack.top(); value_stack.pop();
            ValueUnit lhs = value_stack.top(); value_stack.pop();
            ValueUnit result;

            switch (op) {
                case TokenType::Plus:
                    if (lhs.unit != rhs.unit) throw std::runtime_error("Addition units must be the same.");
                    result = {lhs.value + rhs.value, lhs.unit};
                    break;
                case TokenType::Minus:
                    if (lhs.unit != rhs.unit) throw std::runtime_error("Subtraction units must be the same.");
                    result = {lhs.value - rhs.value, lhs.unit};
                    break;
                case TokenType::Asterisk:
                    if (!lhs.unit.empty() && !rhs.unit.empty()) throw std::runtime_error("Multiplication requires one unitless operand.");
                    result = {lhs.value * rhs.value, lhs.unit.empty() ? rhs.unit : lhs.unit};
                    break;
                case TokenType::Slash:
                    if (rhs.value == 0) throw std::runtime_error("Division by zero.");
                    if (!rhs.unit.empty()) throw std::runtime_error("Division by a value with units is not supported.");
                    result = {lhs.value / rhs.value, lhs.unit};
                    break;
                case TokenType::Percent:
                     if (!rhs.unit.empty()) throw std::runtime_error("Modulo by a value with units is not supported.");
                     result = {fmod(lhs.value, rhs.value), lhs.unit};
                     break;
                case TokenType::DoubleAsterisk:
                    if (!rhs.unit.empty()) throw std::runtime_error("Exponentiation by a value with units is not supported.");
                    result = {pow(lhs.value, rhs.value), lhs.unit};
                    break;
                default:
                    throw std::runtime_error("Unsupported operator in RPN evaluation.");
            }
            value_stack.push(result);
        }
    }

    if (value_stack.size() != 1) {
        throw std::runtime_error("Invalid expression: stack did not resolve to a single value.");
    }

    std::stringstream ss;
    // Check for floating point vs integer
    if (value_stack.top().value == floor(value_stack.top().value)) {
        ss << static_cast<long long>(value_stack.top().value) << value_stack.top().unit;
    } else {
        ss << value_stack.top().value << value_stack.top().unit;
    }

    std::string result = ss.str();
    std::cerr << "Evaluated result: " << result << std::endl; // DEBUG
    return result;
}

} // namespace CHTL
