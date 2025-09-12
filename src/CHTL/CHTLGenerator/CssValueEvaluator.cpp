#include "CssValueEvaluator.h"
#include <stdexcept>
#include <cmath>
#include <iostream>
#include <sstream>

namespace CHTL {

// Main public entry point
std::string CssValueEvaluator::evaluate(const std::vector<Token>& tokens, const PropertyRegistry& registry, const LocalContext& local_context) {
    if (tokens.empty()) {
        return "";
    }

    tokens_ = &tokens;
    registry_ = &registry;
    local_context_ = &local_context;
    current_ = 0;

    try {
        Value result = expression();
        return stringify(result);
    } catch (const std::exception& e) {
        std::cerr << "CSS evaluation error: " << e.what() << ". Falling back to calc()-like string." << std::endl;
        std::stringstream ss;
        bool has_operator = false;
        for(const auto& t : tokens) {
            if (t.type >= TokenType::Plus && t.type <= TokenType::DoublePipe) {
                has_operator = true;
                break;
            }
        }

        if (has_operator) ss << "calc(";
        for (size_t i = 0; i < tokens.size(); ++i) {
            ss << tokens[i].lexeme;
            if (has_operator && i < tokens.size() - 1) ss << " ";
        }
        if (has_operator) ss << ")";
        return ss.str();
    }
}

// expression -> ternary
CssValueEvaluator::Value CssValueEvaluator::expression() {
    return ternary();
}

// ternary -> logical ? ternary : ternary
CssValueEvaluator::Value CssValueEvaluator::ternary() {
    Value expr = logical();

    if (match({TokenType::QuestionMark})) {
        Value thenBranch = ternary();
        if (!match({TokenType::Colon})) {
            throw std::runtime_error("Expected ':' after then branch of ternary expression.");
        }
        Value elseBranch = ternary();
        if (isTruthy(expr)) {
            return thenBranch;
        } else {
            return elseBranch;
        }
    }

    return expr;
}

// logical -> comparison ( ( "&&" | "||" ) comparison )*
CssValueEvaluator::Value CssValueEvaluator::logical() {
    Value expr = comparison();

    while (match({TokenType::DoubleAmpersand, TokenType::DoublePipe})) {
        TokenType op = previous().type;
        Value right = comparison();

        if (op == TokenType::DoublePipe) {
            if (isTruthy(expr)) return expr;
        } else { // &&
            if (!isTruthy(expr)) return expr;
        }
        expr = right;
    }

    return expr;
}

// comparison -> term ( ( ">" | "<" ) term )*
CssValueEvaluator::Value CssValueEvaluator::comparison() {
    Value expr = term();

    while (match({TokenType::GreaterThan, TokenType::LessThan})) {
        TokenType op = previous().type;
        Value right = term();

        if (!std::holds_alternative<ValueUnit>(expr) || !std::holds_alternative<ValueUnit>(right)) {
            throw std::runtime_error("Comparison operands must be numbers.");
        }
        const auto& lhs = std::get<ValueUnit>(expr);
        const auto& rhs = std::get<ValueUnit>(right);

        if (lhs.unit != rhs.unit) {
            throw std::runtime_error("Cannot compare numbers with different units.");
        }

        if (op == TokenType::GreaterThan) expr = lhs.value > rhs.value;
        if (op == TokenType::LessThan) expr = lhs.value < rhs.value;
    }
    return expr;
}

// term -> factor ( ( "-" | "+" ) factor )*
CssValueEvaluator::Value CssValueEvaluator::term() {
    Value expr = factor();
    while (match({TokenType::Minus, TokenType::Plus})) {
        TokenType op = previous().type;
        Value right = factor();

        if (!std::holds_alternative<ValueUnit>(expr) || !std::holds_alternative<ValueUnit>(right)) {
            throw std::runtime_error("Addition/subtraction operands must be numbers.");
        }
        const auto& lhs = std::get<ValueUnit>(expr);
        const auto& rhs = std::get<ValueUnit>(right);

        if (lhs.unit != rhs.unit) throw std::runtime_error("Addition/subtraction units must be the same.");

        if (op == TokenType::Plus) expr = ValueUnit{lhs.value + rhs.value, lhs.unit};
        if (op == TokenType::Minus) expr = ValueUnit{lhs.value - rhs.value, lhs.unit};
    }
    return expr;
}

// factor -> power ( ( "/" | "*" | "%" ) power )*
CssValueEvaluator::Value CssValueEvaluator::factor() {
    Value expr = power();
    while (match({TokenType::Slash, TokenType::Asterisk, TokenType::Percent})) {
        TokenType op = previous().type;
        Value right = power();

        if (!std::holds_alternative<ValueUnit>(expr) || !std::holds_alternative<ValueUnit>(right)) {
            throw std::runtime_error("Multiplication/division operands must be numbers.");
        }
        const auto& lhs = std::get<ValueUnit>(expr);
        const auto& rhs = std::get<ValueUnit>(right);

        if (op == TokenType::Asterisk) {
             if (!lhs.unit.empty() && !rhs.unit.empty()) throw std::runtime_error("Multiplication requires one unitless operand.");
             expr = ValueUnit{lhs.value * rhs.value, lhs.unit.empty() ? rhs.unit : lhs.unit};
        } else if (op == TokenType::Slash) {
            if (rhs.value == 0) throw std::runtime_error("Division by zero.");
            if (!rhs.unit.empty()) throw std::runtime_error("Division by a value with units is not supported.");
            expr = ValueUnit{lhs.value / rhs.value, lhs.unit};
        } else { // Percent
            if (!rhs.unit.empty()) throw std::runtime_error("Modulo by a value with units is not supported.");
            expr = ValueUnit{fmod(lhs.value, rhs.value), lhs.unit};
        }
    }
    return expr;
}

// power -> unary ( "**" unary )*
CssValueEvaluator::Value CssValueEvaluator::power() {
    Value expr = unary();
    while (match({TokenType::DoubleAsterisk})) {
        Value right = unary();
        if (!std::holds_alternative<ValueUnit>(expr) || !std::holds_alternative<ValueUnit>(right)) {
            throw std::runtime_error("Exponentiation operands must be numbers.");
        }
        const auto& lhs = std::get<ValueUnit>(expr);
        const auto& rhs = std::get<ValueUnit>(right);
        if (!rhs.unit.empty()) throw std::runtime_error("Exponentiation by a value with units is not supported.");
        expr = ValueUnit{pow(lhs.value, rhs.value), lhs.unit};
    }
    return expr;
}

// unary -> ( "!" | "-" ) unary | primary
CssValueEvaluator::Value CssValueEvaluator::unary() {
    if (match({TokenType::Minus})) {
        Value right = unary();
        if (!std::holds_alternative<ValueUnit>(right)) {
            throw std::runtime_error("Operand for unary minus must be a number.");
        }
        auto val = std::get<ValueUnit>(right);
        return ValueUnit{-val.value, val.unit};
    }
    return primary();
}

// primary -> NUMBER | STRING | "(" expression ")" | property_reference
CssValueEvaluator::Value CssValueEvaluator::primary() {
    if (match({TokenType::Number})) return parseValueUnit(previous().lexeme);
    if (match({TokenType::StringLiteral, TokenType::UnquotedLiteral})) return previous().lexeme;

    if (match({TokenType::Identifier})) {
        const auto& prev = previous();
        // Case 1: Local property reference (e.g., "width")
        if (local_context_->count(prev.lexeme)) {
            return local_context_->at(prev.lexeme);
        }
        // Case 2: Global property reference (e.g., "#box.width")
        if (check(TokenType::Identifier) && !peek().lexeme.empty() && peek().lexeme[0] == '.') {
            const std::string& selector = prev.lexeme;
            std::string property = advance().lexeme.substr(1);
            if (registry_->count(selector) && registry_->at(selector).count(property)) {
                return registry_->at(selector).at(property);
            } else {
                throw std::runtime_error("Unresolved property reference: " + selector + "." + property);
            }
        }
        // If it's just an identifier, treat it as an unquoted string literal
        return prev.lexeme;
    }

    if (match({TokenType::OpenParen})) {
        Value expr = expression();
        if (!match({TokenType::CloseParen})) {
            throw std::runtime_error("Expected ')' after expression.");
        }
        return expr;
    }

    throw std::runtime_error("Expected expression.");
}


// --- Helper Methods ---

bool CssValueEvaluator::isAtEnd() const {
    return current_ >= tokens_->size() || peek().type == TokenType::EndOfFile;
}

const Token& CssValueEvaluator::peek() const {
    return (*tokens_)[current_];
}

const Token& CssValueEvaluator::previous() const {
    return (*tokens_)[current_ - 1];
}

const Token& CssValueEvaluator::advance() {
    if (!isAtEnd()) current_++;
    return previous();
}

bool CssValueEvaluator::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

bool CssValueEvaluator::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

std::string CssValueEvaluator::stringify(const Value& value) {
    if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    }
    if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    }
    if (std::holds_alternative<ValueUnit>(value)) {
        const auto& vu = std::get<ValueUnit>(value);
        std::stringstream ss;
        if (vu.value == floor(vu.value)) {
            ss << static_cast<long long>(vu.value) << vu.unit;
        } else {
            ss << vu.value << vu.unit;
        }
        return ss.str();
    }
    return "";
}

bool CssValueEvaluator::isTruthy(const Value& value) {
    if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value);
    }
    // Following JavaScript-like truthiness: 0 is false, everything else is true.
    if (std::holds_alternative<ValueUnit>(value)) {
        return std::get<ValueUnit>(value).value != 0;
    }
    // Empty strings are false.
    if (std::holds_alternative<std::string>(value)) {
        return !std::get<std::string>(value).empty();
    }
    return false;
}

CssValueEvaluator::ValueUnit CssValueEvaluator::parseValueUnit(const std::string& lexeme) {
    if (lexeme.empty()) {
        throw std::runtime_error("Cannot parse an empty string to ValueUnit.");
    }
    size_t first_alpha = lexeme.find_first_not_of("-.0123456789");
    if (first_alpha == std::string::npos) {
        return {std::stod(lexeme), ""};
    }
    std::string num_part = lexeme.substr(0, first_alpha);
    std::string unit_part = lexeme.substr(first_alpha);
    return {std::stod(num_part), unit_part};
}

} // namespace CHTL
