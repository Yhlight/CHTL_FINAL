#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

class ElementTemplateUsageNode : public BaseNode {
public:
    explicit ElementTemplateUsageNode(std::string name)
        : m_templateName(std::move(name)) {}

    NodeType GetType() const override { return NodeType::ElementTemplateUsage; }

    const std::string& GetTemplateName() const { return m_templateName; }

private:
    std::string m_templateName;
};

} // namespace CHTL
