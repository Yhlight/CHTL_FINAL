#include "Evaluator.h"
#include "../CHTLNode/Expression/BinaryExprNode.h"
#include "../CHTLNode/Expression/LiteralExprNode.h"
#include <stdexcept>

Value Evaluator::evaluate(std::shared_ptr<ExprNode> expr) {
    expr->accept(*this);
    return lastValue;
}

void Evaluator::visit(LiteralExprNode& expr) {
    // For literals that are not part of an arithmetic expression (like '1px solid black'),
    // the parser now bundles them into a single STRING_LITERAL token.
    if (expr.value.type == TokenType::NUMBER_LITERAL) {
        lastValue = Value::fromString(expr.value.value);
    } else {
        // For any other literal type (IDENTIFIER like 'auto', or a pre-bundled STRING_LITERAL)
        // we treat it as a pure string value.
        lastValue = Value(expr.value.value);
    }
}

void Evaluator::visit(BinaryExprNode& expr) {
    Value left = evaluate(expr.left);
    Value right = evaluate(expr.right);

    // Cannot perform arithmetic on string-only values
    if (left.isString || right.isString) {
        throw std::runtime_error("Cannot perform arithmetic operation on a non-numeric CSS value.");
    }

    // Basic unit check: either units are identical, or one is unitless.
    if (!left.unit.empty() && !right.unit.empty() && left.unit != right.unit) {
        throw std::runtime_error("Mismatched units in expression: " + left.unit + " and " + right.unit);
    }

    // The resulting unit will be the one that is not empty.
    std::string resultUnit = !left.unit.empty() ? left.unit : right.unit;
    double resultNumber = 0.0;

    switch (expr.op.type) {
        case TokenType::PLUS:
            resultNumber = left.number + right.number;
            break;
        case TokenType::MINUS:
            resultNumber = left.number - right.number;
            break;
        case TokenType::STAR:
            if (!left.unit.empty() && !right.unit.empty()) {
                throw std::runtime_error("Cannot multiply two values with units.");
            }
            resultNumber = left.number * right.number;
            break;
        case TokenType::SLASH:
            if (!right.unit.empty()) {
                 throw std::runtime_error("Cannot divide by a value with a unit.");
            }
            if (right.number == 0) {
                 throw std::runtime_error("Division by zero.");
            }
            resultNumber = left.number / right.number;
            break;
        default:
            throw std::runtime_error("Unknown binary operator.");
    }

    lastValue = Value(resultNumber, resultUnit);
}
