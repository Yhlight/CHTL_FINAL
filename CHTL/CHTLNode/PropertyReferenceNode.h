#pragma once

#include "ExpressionNode.h"
#include <string>

namespace CHTL {

class PropertyReferenceNode : public ExpressionNode {
public:
    PropertyReferenceNode(std::string selector, std::string propertyName)
        : m_selector(std::move(selector)), m_propertyName(std::move(propertyName)) {}

    ExpressionNodeType GetType() const override { return ExpressionNodeType::PropertyReference; }

    const std::string& GetSelector() const { return m_selector; }
    const std::string& GetPropertyName() const { return m_propertyName; }

    ExpressionNodePtr Clone() const override {
        return std::make_shared<PropertyReferenceNode>(m_selector, m_propertyName);
    }

private:
    std::string m_selector;
    std::string m_propertyName;
};

} // namespace CHTL
