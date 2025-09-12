#include "Evaluator.h"
#include <cmath> // For pow()

EvaluatedValue Evaluator::evaluate(std::shared_ptr<ExprNode> expr) {
    if (expr) {
        expr->accept(*this);
    } else {
        result.hasError = true;
        result.errorMessage = "Invalid expression tree.";
    }
    return result;
}

void Evaluator::visit(NumericLiteralNode& node) {
    result = {node.value, node.unit, false, ""};
}

void Evaluator::visit(BinaryOpNode& node) {
    EvaluatedValue left = evaluate(node.left);
    if (left.hasError) {
        result = left;
        return;
    }

    EvaluatedValue right = evaluate(node.right);
    if (right.hasError) {
        result = right;
        return;
    }

    // Perform the operation with unit checking
    double newValue = 0;
    std::string newUnit = left.unit; // Default to left unit

    switch (node.op.type) {
        case ValueTokenType::TOKEN_PLUS:
        case ValueTokenType::TOKEN_MINUS:
            if (left.unit != right.unit) {
                result = {0, "", true, "Mismatched units for + or - operation."};
                return;
            }
            newValue = (node.op.type == ValueTokenType::TOKEN_PLUS) ? (left.value + right.value) : (left.value - right.value);
            break;

        case ValueTokenType::TOKEN_STAR:
            if (!left.unit.empty() && !right.unit.empty()) {
                result = {0, "", true, "Cannot multiply two values with units."};
                return;
            }
            newValue = left.value * right.value;
            if (right.unit.empty()) newUnit = left.unit;
            else newUnit = right.unit;
            break;

        case ValueTokenType::TOKEN_SLASH:
             if (!right.unit.empty()) {
                result = {0, "", true, "Cannot divide by a value with units."};
                return;
            }
            if (right.value == 0) {
                 result = {0, "", true, "Division by zero."};
                 return;
            }
            newValue = left.value / right.value;
            break;

        case ValueTokenType::TOKEN_POWER:
            if (!right.unit.empty()) {
                result = {0, "", true, "Exponent must be unitless."};
                return;
            }
            newValue = pow(left.value, right.value);
            break;

        default:
            result = {0, "", true, "Unsupported binary operator."};
            return;
    }

    result = {newValue, newUnit, false, ""};
}
