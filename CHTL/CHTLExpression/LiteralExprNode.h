#ifndef CHTL_LITERAL_EXPR_NODE_H
#define CHTL_LITERAL_EXPR_NODE_H

#include "ExprNode.h"
#include "../CHTLLexer/Token.h"

class LiteralExprNode : public ExprNode {
public:
    Token value;
    explicit LiteralExprNode(const Token& val) : value(val) {}
};

#endif // CHTL_LITERAL_EXPR_NODE_H
