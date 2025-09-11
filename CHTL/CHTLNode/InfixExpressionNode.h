#pragma once

#include "ExpressionNode.h"
#include "../CHTLLexer/Token.h"
#include <sstream>

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

    NodePtr clone() const override {
        auto left_clone = std::dynamic_pointer_cast<ExpressionNode>(left->clone());
        auto right_clone = std::dynamic_pointer_cast<ExpressionNode>(right->clone());
        return std::make_shared<InfixExpressionNode>(left_clone, op, right_clone);
    }

    ExpressionPtr left;
    Token op;
    ExpressionPtr right;
};

} // namespace CHTL
