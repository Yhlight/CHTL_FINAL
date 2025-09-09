#include "Evaluator.h"
#include "../ExpressionNode/TernaryExpr.h"
#include "../ExpressionNode/BinaryExpr.h"
#include "../ExpressionNode/LiteralExpr.h"
#include "../ExpressionNode/VariableExpr.h"
#include "../ExpressionNode/FunctionCallExpr.h"
#include "../CHTLNode/VariableDefinitionNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StylePropertyNode.h"
#include <stdexcept>
#include <cctype>

namespace CHTL {

// --- Helper Functions ---

ChtlValue parseNumericValue(const std::string& s) {
    size_t i = 0;
    while (i < s.length() && (std::isdigit(s[i]) || s[i] == '.')) {
        i++;
    }
    try {
        double number = std::stod(s.substr(0, i));
        std::string unit = s.substr(i);
        return {ChtlValue::Type::Number, false, number, "", unit};
    } catch (...) {
        return {ChtlValue::Type::String, false, 0.0, s, ""};
    }
}

bool isTruthy(const ChtlValue& value) {
    if (value.type == ChtlValue::Type::Null) return false;
    if (value.type == ChtlValue::Type::Bool) return value.boolValue;
    return true;
}

// --- Evaluator Implementation ---

ChtlValue Evaluator::evaluate(const Expr* expr, const CHTLContext& globalContext, const EvaluationContext& localContext, NodeResolver resolver) {
    m_global_context = &globalContext;
    m_local_context = &localContext;
    m_node_resolver = resolver;
    if (expr) {
        expr->accept(*this);
    }
    return m_lastValue;
}

ChtlValue Evaluator::evaluateSubExpr(const Expr* expr) {
    Evaluator subEvaluator;
    return subEvaluator.evaluate(expr, *m_global_context, *m_local_context, m_node_resolver);
}

void Evaluator::visit(const LiteralExpr& expr) {
    const std::string& val = expr.getLiteral().value;
    if (!val.empty() && (std::isdigit(val[0]) || (val[0] == '.' && val.length() > 1 && std::isdigit(val[1])))) {
        m_lastValue = parseNumericValue(val);
    } else {
        m_lastValue = {ChtlValue::Type::String, false, 0.0, val, ""};
    }
}

void Evaluator::visit(const VariableExpr& expr) {
    const std::string& path = expr.getName().value;
    size_t dot_pos = path.find_last_of('.');

    // Case 1: Simple local variable (e.g., "width")
    if (dot_pos == std::string::npos) {
        if (m_local_context->count(path)) {
            m_lastValue = m_local_context->at(path);
        } else {
            m_lastValue = {ChtlValue::Type::String, false, 0.0, path, ""};
        }
        return;
    }

    // Case 2: Cross-element variable (e.g., "#box.width")
    std::string selector = path.substr(0, dot_pos);
    std::string attrName = path.substr(dot_pos + 1);

    const ElementNode* targetNode = m_node_resolver(selector);
    if (!targetNode) {
        throw std::runtime_error("Could not find element with selector: " + selector);
    }

    // Now find the style property on the target node
    if (targetNode->getStyleBlock()) {
        for (const auto& propBase : targetNode->getStyleBlock()->getChildren()) {
            if (propBase->getType() == NodeType::StyleProperty) {
                const auto* prop = static_cast<const StylePropertyNode*>(propBase.get());
                if (prop->getKey() == attrName) {
                    if (!prop->getValues().empty()) {
                         m_lastValue = evaluateSubExpr(prop->getValues()[0].get());
                    } else {
                        m_lastValue = {ChtlValue::Type::Null};
                    }
                    return;
                }
            }
        }
    }

    throw std::runtime_error("Attribute '" + attrName + "' not found on element with selector '" + selector + "'.");
}

void Evaluator::visit(const FunctionCallExpr& expr) {
    auto* calleeAsVar = dynamic_cast<const VariableExpr*>(expr.getCallee());
    if (!calleeAsVar) throw std::runtime_error("Invalid callee type for variable group access.");

    if (expr.getArguments().size() != 1) throw std::runtime_error("Variable group access requires exactly one argument.");

    auto* argAsVar = dynamic_cast<const VariableExpr*>(expr.getArguments()[0].get());
    if (!argAsVar) throw std::runtime_error("Invalid argument type for variable group access.");

    const std::string& groupName = calleeAsVar->getName().value;
    const std::string& varName = argAsVar->getName().value;

    const VariableGroupDefinitionNode* group = m_global_context->getVariableGroup(CHTLContext::GLOBAL_NAMESPACE, groupName);
    if (!group) throw std::runtime_error("Variable group not found: " + groupName);

    for (const auto& varDefNodeBase : group->getVariables()) {
        const auto* varDefNode = static_cast<const VariableDefinitionNode*>(varDefNodeBase.get());
        if (varDefNode->getKey() == varName) {
            m_lastValue = evaluateSubExpr(varDefNode->getValue());
            return;
        }
    }

    throw std::runtime_error("Variable '" + varName + "' not found in group '" + groupName + "'.");
}


void Evaluator::visit(const BinaryExpr& expr) {
    ChtlValue left = evaluateSubExpr(expr.getLeft());

    if (expr.getOperator().type == TokenType::LogicalAnd) {
        if (!isTruthy(left)) { m_lastValue = {ChtlValue::Type::Bool, false}; return; }
        ChtlValue right = evaluateSubExpr(expr.getRight());
        m_lastValue = {ChtlValue::Type::Bool, isTruthy(right)};
        return;
    }

    if (expr.getOperator().type == TokenType::LogicalOr) {
        if (isTruthy(left)) { m_lastValue = {ChtlValue::Type::Bool, true}; return; }
        ChtlValue right = evaluateSubExpr(expr.getRight());
        m_lastValue = {ChtlValue::Type::Bool, isTruthy(right)};
        return;
    }

    ChtlValue right = evaluateSubExpr(expr.getRight());

    if (left.type != ChtlValue::Type::Number || right.type != ChtlValue::Type::Number) {
        m_lastValue = {ChtlValue::Type::Bool, false};
        return;
    }

    bool result = false;
    switch (expr.getOperator().type) {
        case TokenType::GreaterThan: result = left.numberValue > right.numberValue; break;
        case TokenType::LessThan:    result = left.numberValue < right.numberValue; break;
        default: break;
    }
    m_lastValue = {ChtlValue::Type::Bool, result};
}

void Evaluator::visit(const TernaryExpr& expr) {
    ChtlValue condition = evaluateSubExpr(expr.getCondition());
    if (isTruthy(condition)) {
        m_lastValue = evaluateSubExpr(expr.getThenBranch());
    } else {
        if (expr.getElseBranch()) {
            m_lastValue = evaluateSubExpr(expr.getElseBranch());
        } else {
            m_lastValue = {ChtlValue::Type::Null}; // No else branch, result is null
        }
    }
}

}
