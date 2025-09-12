#include "ExpressionEvaluator.h"
#include "CHTL/CHTLNode/BinaryExpr.h"
#include "CHTL/CHTLNode/NumberLiteralExpr.h"
#include "CHTL/CHTLNode/GroupingExpr.h"
#include <stdexcept>

namespace CHTL {

ComputedValue ExpressionEvaluator::evaluate(ExprNode* expr) {
    expr->accept(*this);
    return lastValue;
}

void ExpressionEvaluator::visit(NumberLiteralExpr& expr) {
    lastValue = {expr.getNumberValue(), expr.getUnit()};
}

void ExpressionEvaluator::visit(GroupingExpr& expr) {
    expr.getExpression()->accept(*this);
}

void ExpressionEvaluator::visit(BinaryExpr& expr) {
    // Evaluate left and right sides
    ComputedValue left = evaluate(expr.getLeft());
    ComputedValue right = evaluate(expr.getRight());

    // Basic unit check: for now, for + and -, units must match or one must be empty.
    if (expr.getOp().type == TokenType::PLUS || expr.getOp().type == TokenType::MINUS) {
        if (!left.unit.empty() && !right.unit.empty() && left.unit != right.unit) {
            throw std::runtime_error("Unit mismatch in expression: " + left.unit + " and " + right.unit);
        }
    }

    double resultValue = 0.0;
    switch (expr.getOp().type) {
        case TokenType::PLUS:
            resultValue = left.value + right.value;
            break;
        case TokenType::MINUS:
            resultValue = left.value - right.value;
            break;
        case TokenType::STAR:
            resultValue = left.value * right.value;
            break;
        case TokenType::SLASH:
            if (right.value == 0) throw std::runtime_error("Division by zero.");
            resultValue = left.value / right.value;
            break;
        default:
            throw std::runtime_error("Unknown binary operator.");
    }

    // Determine the resulting unit
    std::string resultUnit = !left.unit.empty() ? left.unit : right.unit;
    if (expr.getOp().type == TokenType::STAR || expr.getOp().type == TokenType::SLASH) {
        if (!left.unit.empty() && !right.unit.empty()) {
            // This is a simplification. Real CSS unit multiplication is complex.
            // For now, we'll just keep the left unit.
            resultUnit = left.unit;
        }
    }

    lastValue = {resultValue, resultUnit};
}

} // namespace CHTL
