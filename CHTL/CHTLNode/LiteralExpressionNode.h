#pragma once

#include "ExpressionNode.h"
#include "../CHTLLexer/Token.h"

namespace CHTL {

class LiteralExpressionNode : public ExpressionNode {
public:
    LiteralExpressionNode(Token token) : token(std::move(token)) {}

    std::string ToString(int indent = 0) const override {
        return std::string(indent, ' ') + "Literal(" + token.literal + ")";
    }

    NodePtr clone() const override {
        return std::make_shared<LiteralExpressionNode>(token);
    }

    Token token;
};

} // namespace CHTL
