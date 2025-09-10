#pragma once

#include "ExpressionNode.h"
#include <string>

namespace CHTL {

class StringLiteralNode : public ExpressionNode {
public:
    explicit StringLiteralNode(std::string value)
        : m_value(std::move(value)) {}

    ExpressionNodeType GetType() const override { return ExpressionNodeType::StringLiteral; }

    const std::string& GetValue() const { return m_value; }

private:
    std::string m_value;
};

} // namespace CHTL
