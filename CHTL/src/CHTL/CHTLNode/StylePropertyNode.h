#pragma once

#include "BaseNode.h"
#include "../ExpressionNode/Expr.h"
#include <string>
#include <memory>
#include <vector>

namespace CHTL {

class StylePropertyNode : public BaseNode {
public:
    StylePropertyNode(std::string key, std::vector<std::unique_ptr<Expr>> valueExprs)
        : m_key(std::move(key)), m_valueExprs(std::move(valueExprs)) {}

    NodeType getType() const override { return NodeType::StyleProperty; }

    std::unique_ptr<BaseNode> clone() const override {
        std::vector<std::unique_ptr<Expr>> cloned_exprs;
        for (const auto& expr : m_valueExprs) {
            cloned_exprs.push_back(expr->clone());
        }
        return std::make_unique<StylePropertyNode>(m_key, std::move(cloned_exprs));
    }

    const std::string& getKey() const { return m_key; }
    const std::vector<std::unique_ptr<Expr>>& getValues() const { return m_valueExprs; }

private:
    std::string m_key;
    std::vector<std::unique_ptr<Expr>> m_valueExprs;
};

} // namespace CHTL
