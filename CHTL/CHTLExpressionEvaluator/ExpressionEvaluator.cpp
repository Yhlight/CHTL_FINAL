#include "ExpressionEvaluator.h"
#include <cmath>
#include <charconv>
#include <algorithm>

// --- Main Evaluation Logic ---

std::string ExpressionEvaluator::evaluate(ExprNode* root) {
    if (!root) return "";
    CssValue result = visit(root);

    // Convert double to string, removing trailing zeros
    std::string valueStr = std::to_string(result.value);
    valueStr.erase(valueStr.find_last_not_of('0') + 1, std::string::npos);
    if (valueStr.back() == '.') {
        valueStr.pop_back();
    }

    return valueStr + result.unit;
}

CssValue ExpressionEvaluator::visit(ExprNode* node) {
    if (auto* literal = dynamic_cast<LiteralExprNode*>(node)) {
        return visitLiteral(literal);
    }
    if (auto* binaryOp = dynamic_cast<BinaryOpExprNode*>(node)) {
        return visitBinaryOp(binaryOp);
    }
    if (auto* unaryOp = dynamic_cast<UnaryOpExprNode*>(node)) {
        return visitUnaryOp(unaryOp);
    }
    throw std::runtime_error("Unknown expression node type.");
}

CssValue ExpressionEvaluator::visitLiteral(LiteralExprNode* node) {
    const std::string& lexeme = node->value.lexeme;

    // Simple unit parsing: find first non-digit char that isn't at the start (for negative numbers)
    auto it = lexeme.begin();
    if (it != lexeme.end() && (*it == '-' || *it == '+')) {
        ++it;
    }
    it = std::find_if(it, lexeme.end(), [](char c){ return !std::isdigit(c) && c != '.'; });

    std::string numPart(lexeme.begin(), it);
    std::string unitPart(it, lexeme.end());

    double value = 0.0;
    if (!numPart.empty()) {
        try {
            value = std::stod(numPart);
        } catch (const std::invalid_argument& ia) {
            // Not a number, treat the whole thing as a unit/keyword
            return { 0, lexeme };
        }
    } else {
        // It's just a unit/keyword like 'bold'
        return { 0, lexeme };
    }

    return { value, unitPart };
}

CssValue ExpressionEvaluator::visitBinaryOp(BinaryOpExprNode* node) {
    CssValue left = visit(node->left.get());
    CssValue right = visit(node->right.get());

    // Simple unit strategy: use the unit of the left operand.
    std::string resultUnit = left.unit.empty() ? right.unit : left.unit;
    double resultValue = 0.0;

    switch (node->op.type) {
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
            if (right.value == 0) throw std::runtime_error("Division by zero in expression.");
            resultValue = left.value / right.value;
            break;
        case TokenType::PERCENT:
            resultValue = fmod(left.value, right.value);
            break;
        case TokenType::STAR_STAR:
            resultValue = pow(left.value, right.value);
            break;
        default:
            throw std::runtime_error("Unsupported binary operator.");
    }

    return { resultValue, resultUnit };
}

CssValue ExpressionEvaluator::visitUnaryOp(UnaryOpExprNode* node) {
    CssValue right = visit(node->right.get());

    if (node->op.type == TokenType::MINUS) {
        right.value = -right.value;
    }
    // Could handle other unary ops like '+' or '!' here in the future.

    return right;
}
