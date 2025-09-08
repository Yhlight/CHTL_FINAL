#pragma once

#include "BaseNode.h"
#include "../ExpressionNode/Expr.h"
#include <string>
#include <memory>

namespace CHTL {

class StylePropertyNode : public BaseNode {
public:
    StylePropertyNode(std::string key, std::unique_ptr<Expr> valueExpr)
        : m_key(std::move(key)), m_valueExpr(std::move(valueExpr)) {}

    NodeType getType() const override { return NodeType::StyleProperty; }

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<StylePropertyNode>(m_key, m_valueExpr->clone());
    }

    const std::string& getKey() const { return m_key; }
    const Expr* getValue() const { return m_valueExpr.get(); }

private:
    std::string m_key;
    std::unique_ptr<Expr> m_valueExpr;
};

} // namespace CHTL
