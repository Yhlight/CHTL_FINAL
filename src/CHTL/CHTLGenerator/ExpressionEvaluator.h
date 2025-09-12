#ifndef CHTL_EXPRESSIONEVALUATOR_H
#define CHTL_EXPRESSIONEVALUATOR_H

#include "CHTL/CHTLNode/ExprVisitor.h"
#include <string>

namespace CHTL {

class ExprNode; // Forward declaration

// Struct to hold the result of an evaluation
struct ComputedValue {
    double value;
    std::string unit;
};

class ExpressionEvaluator : public ExprVisitor {
public:
    ComputedValue evaluate(ExprNode* expr);

    void visit(BinaryExpr& expr) override;
    void visit(NumberLiteralExpr& expr) override;
    void visit(GroupingExpr& expr) override;

private:
    ComputedValue lastValue;
};

} // namespace CHTL

#endif // CHTL_EXPRESSIONEVALUATOR_H
