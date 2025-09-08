#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

// Represents the definition of a template, e.g., `[Template] @Style MyStyles { ... }`
class TemplateDefinitionNode : public BaseNode {
public:
    TemplateDefinitionNode(std::string type, std::string name)
        : m_templateType(std::move(type)), m_templateName(std::move(name)) {}

    NodeType getType() const override { return NodeType::TemplateDefinition; }

    const std::string& getTemplateType() const { return m_templateType; }
    const std::string& getTemplateName() const { return m_templateName; }

    void addChild(std::unique_ptr<BaseNode> child) {
        m_children.push_back(std::move(child));
    }

    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const {
        return m_children;
    }

private:
    std::string m_templateType; // e.g., "Style", "Element", "Var"
    std::string m_templateName;
    std::vector<std::unique_ptr<BaseNode>> m_children; // The content of the template
};

} // namespace CHTL
