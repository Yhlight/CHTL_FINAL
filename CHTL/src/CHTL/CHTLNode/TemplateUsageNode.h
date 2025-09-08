#pragma once

#include "BaseNode.h"
#include <string>
#include <memory>

namespace CHTL {

class TemplateUsageNode : public BaseNode {
public:
    TemplateUsageNode(std::string type, std::string name)
        : m_templateType(std::move(type)), m_templateName(std::move(name)) {}

    NodeType getType() const override { return NodeType::TemplateUsage; }

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<TemplateUsageNode>(m_templateType, m_templateName);
    }

    const std::string& getTemplateType() const { return m_templateType; }
    const std::string& getTemplateName() const { return m_templateName; }

private:
    std::string m_templateType;
    std::string m_templateName;
};

} // namespace CHTL
