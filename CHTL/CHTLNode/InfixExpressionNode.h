#pragma once

#include "ExpressionNode.h"
#include "../CHTLLexer/Token.h"

namespace CHTL {

class InfixExpressionNode : public ExpressionNode {
public:
    InfixExpressionNode(ExpressionPtr left, Token op, ExpressionPtr right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

    std::string ToString(int indent = 0) const override {
        std::stringstream ss;
        ss << std::string(indent, ' ') << "Infix(" << op.literal << ") [\n";
        ss << left->ToString(indent + 2) << "\n";
        ss << right->ToString(indent + 2) << "\n";
        ss << std::string(indent, ' ') << "]";
        return ss.str();
    }

    ExpressionPtr left;
    Token op;
    ExpressionPtr right;
};

} // namespace CHTL
