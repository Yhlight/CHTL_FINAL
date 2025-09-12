#include "ExprPrinter.h"
#include "CHTL/CHTLNode/BinaryExpr.h"
#include "CHTL/CHTLNode/NumberLiteralExpr.h"
#include "CHTL/CHTLNode/GroupingExpr.h"
#include <vector>

namespace CHTL {

std::string ExprPrinter::print(ExprNode* expr) {
    result.clear(); // Reset for each call
    expr->accept(*this);
    return result;
}

void ExprPrinter::visit(BinaryExpr& expr) {
    result += "(";
    result += expr.getOp().lexeme;
    result += " ";
    expr.getLeft()->accept(*this);
    result += " ";
    expr.getRight()->accept(*this);
    result += ")";
}

void ExprPrinter::visit(NumberLiteralExpr& expr) {
    result += std::to_string(expr.getNumberValue());
    result += expr.getUnit();
}

void ExprPrinter::visit(GroupingExpr& expr) {
    result += "(group ";
    expr.getExpression()->accept(*this);
    result += ")";
}

} // namespace CHTL
