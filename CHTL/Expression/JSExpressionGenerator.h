#ifndef JS_EXPRESSION_GENERATOR_H
#define JS_EXPRESSION_GENERATOR_H

#include "Expr.h"
#include <string>
#include <sstream>

namespace CHTL {

class JSExpressionGenerator : public ExprVisitor {
public:
    JSExpressionGenerator();
    std::string generate(Expr* expr);

    void visit(BinaryExpr& expr) override;
    void visit(LiteralExpr& expr) override;
    void visit(VarExpr& expr) override;
    void visit(ReferenceExpr& expr) override;
    void visit(ComparisonExpr& expr) override;
    void visit(LogicalExpr& expr) override;
    void visit(ConditionalExpr& expr) override;
    void visit(DynamicReferenceExpr& expr) override;

private:
    std::stringstream ss;
};

} // namespace CHTL

#endif // JS_EXPRESSION_GENERATOR_H
