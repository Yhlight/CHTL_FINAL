#include "Evaluator.hpp"

namespace CHTL {

Object Evaluator::evaluate(Node* node) {
    if (!node) return {ObjectType::NULL_OBJ};

    if (auto* numNode = dynamic_cast<NumberLiteralNode*>(node)) {
        return {ObjectType::INTEGER, numNode->value};
    }
    if (auto* strNode = dynamic_cast<StringLiteralNode*>(node)) {
        return {ObjectType::STRING, strNode->value};
    }
    if (auto* binNode = dynamic_cast<BinaryExpressionNode*>(node)) {
        Object left = evaluate(binNode->left.get());
        Object right = evaluate(binNode->right.get());
        return evaluateBinaryExpression(binNode->op, left, right);
    }
    if (auto* condNode = dynamic_cast<ConditionalExpressionNode*>(node)) {
        return evaluateConditionalExpression(condNode);
    }

    return {ObjectType::NULL_OBJ};
}

Object Evaluator::evaluateBinaryExpression(TokenType op, Object left, Object right) {
    if (left.type == ObjectType::INTEGER && right.type == ObjectType::INTEGER) {
        int leftVal = std::get<int>(left.value);
        int rightVal = std::get<int>(right.value);
        switch (op) {
            case TokenType::PLUS:     return {ObjectType::INTEGER, leftVal + rightVal};
            case TokenType::MINUS:    return {ObjectType::INTEGER, leftVal - rightVal};
            case TokenType::ASTERISK: return {ObjectType::INTEGER, leftVal * rightVal};
            case TokenType::SLASH:    return {ObjectType::INTEGER, rightVal != 0 ? leftVal / rightVal : 0};
            case TokenType::LT:       return {ObjectType::BOOLEAN, leftVal < rightVal};
            case TokenType::GT:       return {ObjectType::BOOLEAN, leftVal > rightVal};
            default:                  return {ObjectType::NULL_OBJ};
        }
    }
    // Add more type combinations later (e.g., string concatenation)
    return {ObjectType::NULL_OBJ};
}

Object Evaluator::evaluateConditionalExpression(ConditionalExpressionNode* node) {
    Object condition = evaluate(node->condition.get());

    bool isTruthy = false;
    if (condition.type == ObjectType::BOOLEAN) {
        isTruthy = std::get<bool>(condition.value);
    } else if (condition.type == ObjectType::INTEGER) {
        isTruthy = (std::get<int>(condition.value) != 0);
    }
    // Other types are considered false for now

    if (isTruthy) {
        return evaluate(node->consequence.get());
    } else {
        return evaluate(node->alternative.get());
    }
}

} // namespace CHTL
