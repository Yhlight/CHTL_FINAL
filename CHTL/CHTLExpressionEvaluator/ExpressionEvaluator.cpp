#include "ExpressionEvaluator.h"
#include <cmath>
#include <charconv>
#include <algorithm>

std::string ExpressionEvaluator::evaluate(ExprNode* root, const PropertyMap& context) {
    if (!root) return "";
    CssValue result = visit(root, context);

    if (std::holds_alternative<double>(result.value)) {
        double val = std::get<double>(result.value);
        std::string valueStr = std::to_string(val);
        valueStr.erase(valueStr.find_last_not_of('0') + 1, std::string::npos);
        if (valueStr.back() == '.') valueStr.pop_back();
        return valueStr + result.unit;
    } else if (std::holds_alternative<std::string>(result.value)) {
        return std::get<std::string>(result.value);
    } else if (std::holds_alternative<bool>(result.value)) {
        return std::get<bool>(result.value) ? "true" : "false";
    }
    return "";
}

CssValue ExpressionEvaluator::visit(ExprNode* node, const PropertyMap& context) {
    if (auto* literal = dynamic_cast<LiteralExprNode*>(node)) return visitLiteral(literal, context);
    if (auto* binaryOp = dynamic_cast<BinaryOpExprNode*>(node)) return visitBinaryOp(binaryOp, context);
    if (auto* unaryOp = dynamic_cast<UnaryOpExprNode*>(node)) return visitUnaryOp(unaryOp, context);
    if (auto* cond = dynamic_cast<ConditionalExprNode*>(node)) return visitConditional(cond, context);
    throw std::runtime_error("Unknown expression node type.");
}

CssValue ExpressionEvaluator::visitLiteral(LiteralExprNode* node, const PropertyMap& context) {
    const std::string& lexeme = node->value.lexeme;

    if (context.count(lexeme)) {
        return context.at(lexeme);
    }

    auto it = lexeme.begin();
    if (it != lexeme.end() && (*it == '-' || *it == '+')) ++it;
    it = std::find_if(it, lexeme.end(), [](char c){ return !std::isdigit(c) && c != '.'; });

    std::string numPart(lexeme.begin(), it);
    std::string unitPart(it, lexeme.end());

    if (numPart.empty()) return { lexeme, "" };

    try {
        double value = std::stod(numPart);
        return { value, unitPart };
    } catch (const std::invalid_argument&) {
        return { lexeme, "" };
    }
}

CssValue ExpressionEvaluator::visitBinaryOp(BinaryOpExprNode* node, const PropertyMap& context) {
    CssValue left = visit(node->left.get(), context);
    CssValue right = visit(node->right.get(), context);

    if (node->op.type == TokenType::GREATER || node->op.type == TokenType::LESS) {
        if (!std::holds_alternative<double>(left.value) || !std::holds_alternative<double>(right.value)) {
            throw std::runtime_error("Cannot perform comparison on non-numeric values.");
        }
        bool result = (node->op.type == TokenType::GREATER) ?
            (std::get<double>(left.value) > std::get<double>(right.value)) :
            (std::get<double>(left.value) < std::get<double>(right.value));
        return CssValue{ EvalResult(result), "" };
    }

    if (!std::holds_alternative<double>(left.value) || !std::holds_alternative<double>(right.value)) {
        throw std::runtime_error("Cannot perform arithmetic on non-numeric CSS values.");
    }

    std::string resultUnit = !left.unit.empty() ? left.unit : right.unit;
    double resultValue = 0.0;
    double leftVal = std::get<double>(left.value);
    double rightVal = std::get<double>(right.value);

    switch (node->op.type) {
        case TokenType::PLUS: resultValue = leftVal + rightVal; break;
        case TokenType::MINUS: resultValue = leftVal - rightVal; break;
        case TokenType::STAR: resultValue = leftVal * rightVal; break;
        case TokenType::SLASH:
            if (rightVal == 0) throw std::runtime_error("Division by zero in expression.");
            resultValue = leftVal / rightVal;
            break;
        case TokenType::PERCENT: resultValue = fmod(leftVal, rightVal); break;
        case TokenType::STAR_STAR: resultValue = pow(leftVal, rightVal); break;
        default: throw std::runtime_error("Unsupported binary operator for arithmetic.");
    }

    return { resultValue, resultUnit };
}

CssValue ExpressionEvaluator::visitUnaryOp(UnaryOpExprNode* node, const PropertyMap& context) {
    CssValue right = visit(node->right.get(), context);

    if (!std::holds_alternative<double>(right.value)) {
        throw std::runtime_error("Cannot apply unary minus to a non-numeric CSS value.");
    }

    if (node->op.type == TokenType::MINUS) {
        return { -std::get<double>(right.value), right.unit };
    }
    return right;
}

CssValue ExpressionEvaluator::visitConditional(ConditionalExprNode* node, const PropertyMap& context) {
    CssValue conditionResult = visit(node->condition.get(), context);
    if (!std::holds_alternative<bool>(conditionResult.value)) {
        throw std::runtime_error("Condition in ternary expression did not evaluate to a boolean.");
    }

    if (std::get<bool>(conditionResult.value)) {
        return visit(node->thenBranch.get(), context);
    } else {
        return visit(node->elseBranch.get(), context);
    }
}
