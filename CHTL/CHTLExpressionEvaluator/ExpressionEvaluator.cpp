#include "ExpressionEvaluator.h"
#include <cmath>
#include <charconv>
#include <algorithm>
#include <iostream>

std::string ExpressionEvaluator::evaluate(ExprNode* root, const PropertyMap& localContext) {
    if (!root) return "";
    CssValue result = visit(root, localContext);
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

CssValue ExpressionEvaluator::visit(ExprNode* node, const PropertyMap& localContext) {
    if (auto* literal = dynamic_cast<LiteralExprNode*>(node)) return visitLiteral(literal, localContext);
    if (auto* binaryOp = dynamic_cast<BinaryOpExprNode*>(node)) return visitBinaryOp(binaryOp, localContext);
    if (auto* unaryOp = dynamic_cast<UnaryOpExprNode*>(node)) return visitUnaryOp(unaryOp, localContext);
    if (auto* cond = dynamic_cast<ConditionalExprNode*>(node)) return visitConditional(cond, localContext);
    throw std::runtime_error("Unknown expression node type.");
}

CssValue ExpressionEvaluator::visitLiteral(LiteralExprNode* node, const PropertyMap& localContext) {
    const std::string& lexeme = node->value.lexeme;
    if (localContext.count(lexeme)) return localContext.at(lexeme);
    auto it = lexeme.begin();
    if (it != lexeme.end() && (*it == '-' || *it == '+')) ++it;
    it = std::find_if(it, lexeme.end(), [](char c){ return !std::isdigit(c) && c != '.'; });
    std::string numPart(lexeme.begin(), it);
    std::string unitPart(it, lexeme.end());
    if (numPart.empty()) return { lexeme, "" };
    try {
        return { std::stod(numPart), unitPart };
    } catch (const std::invalid_argument&) {
        return { lexeme, "" };
    }
}

CssValue ExpressionEvaluator::visitBinaryOp(BinaryOpExprNode* node, const PropertyMap& localContext) {
    CssValue left = visit(node->left.get(), localContext);
    CssValue right = visit(node->right.get(), localContext);
    if (node->op.type == TokenType::GREATER || node->op.type == TokenType::LESS) {
    if (!std::holds_alternative<double>(left.value) || !std::holds_alternative<double>(right.value)) throw std::runtime_error("Cannot compare non-numeric values.");
        bool result = (node->op.type == TokenType::GREATER) ? (std::get<double>(left.value) > std::get<double>(right.value)) : (std::get<double>(left.value) < std::get<double>(right.value));
        return CssValue{ EvalResult(result), "" };
    }
    if (!std::holds_alternative<double>(left.value) || !std::holds_alternative<double>(right.value)) throw std::runtime_error("Cannot perform arithmetic on non-numeric values.");
    std::string resultUnit = !left.unit.empty() ? left.unit : right.unit;
    double leftVal = std::get<double>(left.value);
    double rightVal = std::get<double>(right.value);
    double resultValue = 0.0;
    switch (node->op.type) {
        case TokenType::PLUS: resultValue = leftVal + rightVal; break;
        case TokenType::MINUS: resultValue = leftVal - rightVal; break;
        case TokenType::STAR: resultValue = leftVal * rightVal; break;
        case TokenType::SLASH: if (rightVal == 0) throw std::runtime_error("Division by zero."); resultValue = leftVal / rightVal; break;
        case TokenType::PERCENT: resultValue = fmod(leftVal, rightVal); break;
        case TokenType::STAR_STAR: resultValue = pow(leftVal, rightVal); break;
        default: throw std::runtime_error("Unsupported binary operator.");
    }
    return { resultValue, resultUnit };
}

CssValue ExpressionEvaluator::visitUnaryOp(UnaryOpExprNode* node, const PropertyMap& localContext) {
    CssValue right = visit(node->right.get(), localContext);
    if (!std::holds_alternative<double>(right.value)) throw std::runtime_error("Cannot apply unary minus to non-numeric value.");
    if (node->op.type == TokenType::MINUS) return { -std::get<double>(right.value), right.unit };
    return right;
}

CssValue ExpressionEvaluator::visitConditional(ConditionalExprNode* node, const PropertyMap& localContext) {
    CssValue conditionResult = visit(node->condition.get(), localContext);
    if (!std::holds_alternative<bool>(conditionResult.value)) {
        throw std::runtime_error("Condition did not evaluate to a boolean.");
    }
    if (std::get<bool>(conditionResult.value)) return visit(node->thenBranch.get(), localContext);
    else return visit(node->elseBranch.get(), localContext);
}
