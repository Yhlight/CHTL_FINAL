#ifndef CHTL_BINARYEXPR_H
#define CHTL_BINARYEXPR_H

#include "ExprNode.h"
#include "CHTL/CHTLLexer/Token.h"
#include <memory>

namespace CHTL {

class BinaryExpr : public ExprNode {
public:
    BinaryExpr(std::unique_ptr<ExprNode> left, Token op, std::unique_ptr<ExprNode> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}

    void accept(ExprVisitor& visitor) override;

    ExprNode* getLeft() const { return left.get(); }
    Token getOp() const { return op; }
    ExprNode* getRight() const { return right.get(); }

private:
    std::unique_ptr<ExprNode> left;
    Token op;
    std::unique_ptr<ExprNode> right;
};

} // namespace CHTL

#endif // CHTL_BINARYEXPR_H
