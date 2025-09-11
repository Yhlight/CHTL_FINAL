#pragma once

#include "ExpressionNode.h"
#include <string>

namespace CHTL {

class NumberLiteralNode : public ExpressionNode {
public:
    NumberLiteralNode(double value, std::string unit)
        : m_value(value), m_unit(std::move(unit)) {}

    ExpressionNodeType GetType() const override { return ExpressionNodeType::NumberLiteral; }

    double GetValue() const { return m_value; }
    const std::string& GetUnit() const { return m_unit; }

    ExpressionNodePtr Clone() const override {
        return std::make_shared<NumberLiteralNode>(m_value, m_unit);
    }

private:
    double m_value;
    std::string m_unit;
};

} // namespace CHTL
