#include "ExpressionEvaluator.h"

CssValue ExpressionEvaluator::evaluate(ExpressionNode& node) {
    // Ensure the stack is empty before a new evaluation.
    std::stack<CssValue> empty_stack;
    value_stack.swap(empty_stack);

    node.accept(*this);

    // The final result is the only value left on the stack.
    if (value_stack.size() == 1) {
        return value_stack.top();
    }

    // This can happen if the expression is just a number, which is valid.
    if (value_stack.empty()) {
        // This case should be handled by the DimensionNode visitor.
        // If we get here, something is wrong with the AST or the traversal.
         throw std::runtime_error("Expression evaluation ended with an empty stack.");
    }

    throw std::runtime_error("Invalid expression evaluation state: stack has more than one value.");
}

void ExpressionEvaluator::visit(DimensionNode& node) {
    try {
        double value = std::stod(node.number.lexeme);
        std::string unit = node.unit.lexeme;
        value_stack.push({value, unit});
    } catch (const std::invalid_argument& e) {
        throw std::runtime_error("Invalid number format in dimension: " + node.number.lexeme);
    }
}

void ExpressionEvaluator::visit(BinaryOpNode& node) {
    node.left->accept(*this);
    node.right->accept(*this);

    if (value_stack.size() < 2) {
        throw std::runtime_error("Insufficient values on stack for binary operation.");
    }

    CssValue right = value_stack.top();
    value_stack.pop();
    CssValue left = value_stack.top();
    value_stack.pop();

    CssValue result;

    // Unit compatibility checks
    if (node.op.type == TokenType::PLUS || node.op.type == TokenType::MINUS) {
        if (left.unit != right.unit) {
            throw std::runtime_error("Unit mismatch for + or - operation: '" + left.unit + "' and '" + right.unit + "'.");
        }
        result.unit = left.unit;
    } else if (node.op.type == TokenType::STAR || node.op.type == TokenType::SLASH) {
        // For multiplication/division, one side must be unitless.
        if (!left.unit.empty() && !right.unit.empty()) {
            throw std::runtime_error("Cannot multiply or divide two units ('" + left.unit + "' and '" + right.unit + "').");
        }
        result.unit = !left.unit.empty() ? left.unit : right.unit;
    }

    switch (node.op.type) {
        case TokenType::PLUS:  result.value = left.value + right.value; break;
        case TokenType::MINUS: result.value = left.value - right.value; break;
        case TokenType::STAR:  result.value = left.value * right.value; break;
        case TokenType::SLASH:
            if (right.value == 0) throw std::runtime_error("Division by zero.");
            result.value = left.value / right.value;
            break;
        default:
            throw std::runtime_error("Unknown binary operator in expression.");
    }

    value_stack.push(result);
}
