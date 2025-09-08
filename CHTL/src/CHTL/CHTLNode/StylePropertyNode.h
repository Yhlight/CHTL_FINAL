#pragma once

#include "BaseNode.h"
#include "../ExpressionNode/Expr.h"
#include <string>
#include <memory>

namespace CHTL {

// Represents a single CSS-like property within a style block.
// Its value is now represented by an expression tree, which allows for
// both simple literal values and complex conditional expressions.
class StylePropertyNode : public BaseNode {
public:
    StylePropertyNode(std::string key, std::unique_ptr<Expr> valueExpr)
        : m_key(std::move(key)), m_valueExpr(std::move(valueExpr)) {}

    NodeType getType() const override { return NodeType::StyleProperty; }

    const std::string& getKey() const { return m_key; }
    const Expr* getValue() const { return m_valueExpr.get(); }

private:
    std::string m_key;
    std::unique_ptr<Expr> m_valueExpr;
};

} // namespace CHTL
