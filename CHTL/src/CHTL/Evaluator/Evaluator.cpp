#include "Evaluator.h"
#include "../ExpressionNode/TernaryExpr.h"
#include "../ExpressionNode/BinaryExpr.h"
#include "../ExpressionNode/LiteralExpr.h"
#include "../ExpressionNode/VariableExpr.h"
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

ChtlValue Evaluator::evaluate(const Expr* expr, const EvaluationContext& context) {
    m_context = &context;
    if (expr) {
        expr->accept(*this);
    }
    return m_lastValue;
}

ChtlValue Evaluator::evaluateSubExpr(const Expr* expr) {
    Evaluator subEvaluator;
    return subEvaluator.evaluate(expr, *m_context);
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
    const std::string& name = expr.getName().value;
    if (m_context->count(name)) {
        m_lastValue = m_context->at(name);
    } else {
        // If a variable is not in the context, it's treated as a string literal.
        // This handles cases like `color: red` where "red" is parsed as a variable.
        m_lastValue = {ChtlValue::Type::String, false, 0.0, name, ""};
    }
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
        m_lastValue = evaluateSubExpr(expr.getElseBranch());
    }
}

} // namespace CHTL
