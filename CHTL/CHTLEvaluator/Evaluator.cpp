#include "Evaluator.h"
#include <stdexcept>
#include <sstream> // Explicitly include for stringstream usage in included headers

std::string Evaluator::evaluate(ExpressionNode* node) {
    if (!node) {
        return "";
    }

    if (auto literalNode = dynamic_cast<const LiteralNode*>(node)) {
        return evaluateLiteralNode(literalNode);
    }
    if (auto binaryOpNode = dynamic_cast<const BinaryOpNode*>(node)) {
        return evaluateBinaryOpNode(binaryOpNode);
    }

    return ""; // Unknown expression type
}

std::string Evaluator::evaluateLiteralNode(const LiteralNode* node) {
    return node->token.lexeme;
}

std::string Evaluator::evaluateBinaryOpNode(const BinaryOpNode* node) {
    std::string leftValStr = evaluate(node->left.get());
    std::string rightValStr = evaluate(node->right.get());

    // Major Simplification: For now, assume values are numeric and ignore units.
    // This is NOT a correct CSS implementation, but a placeholder for the evaluation logic.
    // A real implementation would need a CSS value type system (e.g., Value(100, "px")).
    try {
        double leftNum = std::stod(leftValStr);
        double rightNum = std::stod(rightValStr);
        double result = 0.0;

        switch (node->op.type) {
            case TokenType::PLUS:
                result = leftNum + rightNum;
                break;
            case TokenType::MINUS:
                result = leftNum - rightNum;
                break;
            case TokenType::ASTERISK:
                result = leftNum * rightNum;
                break;
            case TokenType::SLASH:
                if (rightNum == 0) return "0"; // Avoid division by zero
                result = leftNum / rightNum;
                break;
            default:
                return ""; // Unsupported operator
        }
        // Convert result back to string, removing trailing zeros
        std::string resultStr = std::to_string(result);
        resultStr.erase(resultStr.find_last_not_of('0') + 1, std::string::npos);
        if (resultStr.back() == '.') {
            resultStr.pop_back();
        }
        return resultStr;

    } catch (const std::invalid_argument& e) {
        // If conversion to double fails, just concatenate strings (e.g., for "1px solid black")
        // This is also a simplification.
        if (node->op.type == TokenType::PLUS) {
             return leftValStr + " " + rightValStr;
        }
        return leftValStr + rightValStr;
    }
}
