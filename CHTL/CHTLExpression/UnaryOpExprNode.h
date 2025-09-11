#ifndef CHTL_UNARY_OP_EXPR_NODE_H
#define CHTL_UNARY_OP_EXPR_NODE_H

#include "ExprNode.h"
#include "../CHTLLexer/Token.h"
#include <memory>

class UnaryOpExprNode : public ExprNode {
public:
    Token op;
    std::unique_ptr<ExprNode> right;

    UnaryOpExprNode(const Token& o, std::unique_ptr<ExprNode> r)
        : op(o), right(std::move(r)) {}
};

#endif // CHTL_UNARY_OP_EXPR_NODE_H
