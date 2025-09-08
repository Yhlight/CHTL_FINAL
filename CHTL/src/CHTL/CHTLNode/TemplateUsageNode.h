#pragma once

#include "BaseNode.h"
#include <string>
#include <memory>

namespace CHTL {

class TemplateUsageNode : public BaseNode {
public:
    TemplateUsageNode(std::string type, std::string name, std::string ns = "")
        : m_templateType(std::move(type)), m_templateName(std::move(name)), m_namespace(std::move(ns)) {}

    NodeType getType() const override { return NodeType::TemplateUsage; }

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<TemplateUsageNode>(m_templateType, m_templateName, m_namespace);
    }

    const std::string& getTemplateType() const { return m_templateType; }
    const std::string& getTemplateName() const { return m_templateName; }
    const std::string& getNamespace() const { return m_namespace; }

private:
    std::string m_templateType;
    std::string m_templateName;
    std::string m_namespace;
};

} // namespace CHTL
