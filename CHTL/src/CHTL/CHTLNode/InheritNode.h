#pragma once

#include "BaseNode.h"
#include <string>
#include <memory>

namespace CHTL {

class InheritNode : public BaseNode {
public:
    InheritNode(std::string type, std::string name, std::string ns = "")
        : m_templateType(std::move(type)), m_templateName(std::move(name)), m_namespace(std::move(ns)) {}

    NodeType getType() const override { return NodeType::Inherit; }

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<InheritNode>(m_templateType, m_templateName, m_namespace);
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
