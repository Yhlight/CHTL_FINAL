#ifndef CHTL_EXPRVISITOR_H
#define CHTL_EXPRVISITOR_H

namespace CHTL {

// Forward declare all expression node types
class BinaryExpr;
class NumberLiteralExpr;
class GroupingExpr;

// The ExprVisitor interface for the Visitor design pattern.
// This will be implemented by the ExpressionEvaluator and ExpressionPrinter.
class ExprVisitor {
public:
    virtual ~ExprVisitor() = default;

    virtual void visit(BinaryExpr& expr) = 0;
    virtual void visit(NumberLiteralExpr& expr) = 0;
    virtual void visit(GroupingExpr& expr) = 0;
};

} // namespace CHTL

#endif // CHTL_EXPRVISITOR_H
