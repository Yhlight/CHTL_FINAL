#ifndef CHTL_CONDITIONAL_EXPR_NODE_H
#define CHTL_CONDITIONAL_EXPR_NODE_H

#include "ExprNode.h"
#include <memory>

class ConditionalExprNode : public ExprNode {
public:
    std::unique_ptr<ExprNode> condition;
    std::unique_ptr<ExprNode> thenBranch;
    std::unique_ptr<ExprNode> elseBranch;

    ConditionalExprNode(std::unique_ptr<ExprNode> c, std::unique_ptr<ExprNode> t, std::unique_ptr<ExprNode> e)
        : condition(std::move(c)), thenBranch(std::move(t)), elseBranch(std::move(e)) {}
};

#endif // CHTL_CONDITIONAL_EXPR_NODE_H
