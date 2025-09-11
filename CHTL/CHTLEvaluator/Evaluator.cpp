#include "Evaluator.h"
#include "../CHTLNode/CSSPropertyNode.h"
#include <stdexcept>
#include <sstream>

Evaluator::Evaluator(const TemplateStore& templateStore, const std::string& current_ns)
    : m_templateStore(templateStore), m_currentNamespace(current_ns) {}

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
    if (auto callNode = dynamic_cast<const CallExpressionNode*>(node)) {
        return evaluateCallExpression(callNode);
    }

    return ""; // Unknown expression type
}

std::string Evaluator::evaluateLiteralNode(const LiteralNode* node) {
    return node->token.lexeme;
}

std::string Evaluator::evaluateBinaryOpNode(const BinaryOpNode* node) {
    std::string leftValStr = evaluate(node->left.get());
    std::string rightValStr = evaluate(node->right.get());

    try {
        double leftNum = std::stod(leftValStr);
        double rightNum = std::stod(rightValStr);
        double result = 0.0;

        switch (node->op.type) {
            case TokenType::PLUS: result = leftNum + rightNum; break;
            case TokenType::MINUS: result = leftNum - rightNum; break;
            case TokenType::ASTERISK: result = leftNum * rightNum; break;
            case TokenType::SLASH: result = (rightNum == 0) ? 0 : leftNum / rightNum; break;
            default: return "";
        }
        std::string resultStr = std::to_string(result);
        resultStr.erase(resultStr.find_last_not_of('0') + 1, std::string::npos);
        if (resultStr.back() == '.') {
            resultStr.pop_back();
        }
        return resultStr;

    } catch (const std::invalid_argument& e) {
        if (node->op.type == TokenType::PLUS) {
             return leftValStr + " " + rightValStr;
        }
        return leftValStr + rightValStr;
    }
}

std::string Evaluator::evaluateCallExpression(const CallExpressionNode* node) {
    auto funcNameNode = dynamic_cast<const LiteralNode*>(node->function.get());
    if (!funcNameNode) {
        return "";
    }

    std::string templateName = funcNameNode->token.lexeme;
    auto templateDef = m_templateStore.get(m_currentNamespace, templateName);

    if (!templateDef || templateDef->type != TemplateType::VAR || node->arguments.size() != 1) {
        return "";
    }

    auto argNode = dynamic_cast<const LiteralNode*>(node->arguments[0].get());
    if (!argNode) {
        return "";
    }
    std::string varName = argNode->token.lexeme;

    for (const auto& varProp : templateDef->body) {
        if (auto cssProp = dynamic_cast<const CSSPropertyNode*>(varProp.get())) {
            if (cssProp->propertyName == varName) {
                return evaluate(cssProp->value.get());
            }
        }
    }

    return "";
}
