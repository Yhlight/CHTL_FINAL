#ifndef CHTL_GROUPINGEXPR_H
#define CHTL_GROUPINGEXPR_H

#include "ExprNode.h"
#include <memory>

namespace CHTL {

class GroupingExpr : public ExprNode {
public:
    explicit GroupingExpr(std::unique_ptr<ExprNode> expression)
        : expression(std::move(expression)) {}

    void accept(ExprVisitor& visitor) override;

    ExprNode* getExpression() const { return expression.get(); }

private:
    std::unique_ptr<ExprNode> expression;
};

} // namespace CHTL

#endif // CHTL_GROUPINGEXPR_H
