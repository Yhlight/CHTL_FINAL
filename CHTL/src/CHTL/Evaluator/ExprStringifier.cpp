#include "ExprStringifier.h"
#include "../ExpressionNode/TernaryExpr.h"
#include "../ExpressionNode/BinaryExpr.h"
#include "../ExpressionNode/LiteralExpr.h"
#include "../ExpressionNode/VariableExpr.h"

namespace CHTL {

std::string ExprStringifier::stringify(const Expr* expr) {
    if (expr) {
        expr->accept(*this);
    }
    return m_result;
}

void ExprStringifier::visit(const TernaryExpr& expr) {
    m_result += "(? ";
    expr.getCondition()->accept(*this);
    m_result += " ";
    expr.getThenBranch()->accept(*this);
    m_result += " ";
    expr.getElseBranch()->accept(*this);
    m_result += ")";
}

void ExprStringifier::visit(const BinaryExpr& expr) {
    m_result += "(" + expr.getOperator().value + " ";
    expr.getLeft()->accept(*this);
    m_result += " ";
    expr.getRight()->accept(*this);
    m_result += ")";
}

void ExprStringifier::visit(const LiteralExpr& expr) {
    m_result += expr.getLiteral().value;
}

void ExprStringifier::visit(const VariableExpr& expr) {
    m_result += expr.getName().value;
}

} // namespace CHTL
