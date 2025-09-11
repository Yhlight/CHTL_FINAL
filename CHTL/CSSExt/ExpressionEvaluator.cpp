#include "ExpressionEvaluator.hpp"
#include <stdexcept>
#include <cctype>
#include <algorithm>
#include <cmath>

namespace CHTL {
namespace CSSExt {

// Helper to parse a string like "100px" into a number and a unit.
Value parseLiteralValue(const std::string& str) {
    Value val;
    std::string num_part;
    size_t i = 0;
    while (i < str.length() && (isdigit(str[i]) || str[i] == '.')) {
        num_part += str[i];
        i++;
    }

    if (!num_part.empty()) {
        try {
            val.number = std::stod(num_part);
            val.is_number = true;
            val.unit = str.substr(i);
        } catch (const std::invalid_argument& ia) {
            val.is_number = false;
        }
    }

    if (!val.is_number) {
        val.string_val = str;
    }
    return val;
}


Value ExpressionEvaluator::evaluate(const ExprPtr& expr) {
    expr->accept(*this);
    return m_result;
}

void ExpressionEvaluator::visit(Binary& expr) {
    Value left = evaluate(expr.left);
    Value right = evaluate(expr.right);

    checkNumberOperands(expr.op, left, right);

    Value result;
    result.is_number = true;
    // For simplicity, the result takes the unit of the left operand.
    // A real implementation would need more complex unit logic.
    result.unit = left.unit;

    switch (expr.op.type) {
        case TokenType::MINUS: result.number = left.number - right.number; break;
        case TokenType::PLUS:  result.number = left.number + right.number; break;
        case TokenType::SLASH: result.number = left.number / right.number; break;
        case TokenType::STAR:  result.number = left.number * right.number; break;
        case TokenType::PERCENT: result.number = fmod(left.number, right.number); break;
        case TokenType::STAR_STAR: result.number = pow(left.number, right.number); break;
        default:
            // Should not happen
            break;
    }
    m_result = result;
}

void ExpressionEvaluator::visit(Grouping& expr) {
    m_result = evaluate(expr.expression);
}

void ExpressionEvaluator::visit(Literal& expr) {
    m_result = parseLiteralValue(expr.value.lexeme);
}

void ExpressionEvaluator::visit(Unary& expr) {
    Value right = evaluate(expr.right);
    if (!right.is_number) {
        throw std::runtime_error("Unary '-' operator can only be applied to numbers.");
    }

    if (expr.op.type == TokenType::MINUS) {
        right.number = -right.number;
    }
    m_result = right;
}

void ExpressionEvaluator::checkNumberOperands(const Token& op, const Value& left, const Value& right) {
    if (left.is_number && right.is_number) {
        // Basic check: if both have units, they must be the same.
        if (!left.unit.empty() && !right.unit.empty() && left.unit != right.unit) {
            throw std::runtime_error("Mismatched units in operation '" + op.lexeme + "'");
        }
        return;
    }
    std::string lhs_val = left.is_number ? std::to_string(left.number) + left.unit : left.string_val;
    std::string rhs_val = right.is_number ? std::to_string(right.number) + right.unit : right.string_val;
    throw std::runtime_error("Operands must be numbers for operation '" + op.lexeme + "'. "
        + "LHS is '" + lhs_val + "' (is_number: " + (left.is_number ? "true" : "false") + "), "
        + "RHS is '" + rhs_val + "' (is_number: " + (right.is_number ? "true" : "false") + ")"
    );
}


} // namespace CSSExt
} // namespace CHTL
