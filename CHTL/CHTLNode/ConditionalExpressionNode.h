#pragma once

#include "ExpressionNode.h"
#include <sstream>

namespace CHTL {

class ConditionalExpressionNode : public ExpressionNode {
public:
    ConditionalExpressionNode(ExpressionPtr condition, ExpressionPtr consequence, ExpressionPtr alternative)
        : condition(std::move(condition)),
          consequence(std::move(consequence)),
          alternative(std::move(alternative)) {}

    std::string ToString(int indent = 0) const override {
        std::stringstream ss;
        ss << std::string(indent, ' ') << "ConditionalExpression(?) [\n";
        ss << std::string(indent + 2, ' ') << "Condition:\n";
        ss << condition->ToString(indent + 4) << "\n";
        ss << std::string(indent + 2, ' ') << "Consequence:\n";
        ss << consequence->ToString(indent + 4) << "\n";
        ss << std::string(indent + 2, ' ') << "Alternative:\n";
        ss << alternative->ToString(indent + 4) << "\n";
        ss << std::string(indent, ' ') << "]";
        return ss.str();
    }

    ExpressionPtr condition;
    ExpressionPtr consequence;
    ExpressionPtr alternative;
};

} // namespace CHTL
