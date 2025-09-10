#pragma once

#include "ExpressionNode.h"
#include <string>

namespace CHTL {

class SelfPropertyReferenceNode : public ExpressionNode {
public:
    explicit SelfPropertyReferenceNode(std::string propertyName)
        : m_propertyName(std::move(propertyName)) {}

    ExpressionNodeType GetType() const override { return ExpressionNodeType::SelfPropertyReference; }

    const std::string& GetPropertyName() const { return m_propertyName; }

private:
    std::string m_propertyName;
};

} // namespace CHTL
