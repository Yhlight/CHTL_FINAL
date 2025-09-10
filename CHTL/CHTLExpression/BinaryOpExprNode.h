#ifndef CHTL_BINARY_OP_EXPR_NODE_H
#define CHTL_BINARY_OP_EXPR_NODE_H

#include "ExprNode.h"
#include "../CHTLLexer/Token.h"
#include <memory>

// Represents a binary operation in an expression (e.g., left + right).
class BinaryOpExprNode : public ExprNode {
public:
    std::unique_ptr<ExprNode> left;
    Token op;
    std::unique_ptr<ExprNode> right;

    BinaryOpExprNode(std::unique_ptr<ExprNode> l, const Token& o, std::unique_ptr<ExprNode> r)
        : left(std::move(l)), op(o), right(std::move(r)) {}
};

#endif // CHTL_BINARY_OP_EXPR_NODE_H
