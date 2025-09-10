#pragma once

#include "BaseNode.h"
#include "StyleNode.h" // For now, we only support style templates
#include <string>
#include <memory>

namespace CHTL {

enum class TemplateType {
    Style,
    Element,
    Var
};

class TemplateDefinitionNode : public BaseNode {
public:
    TemplateDefinitionNode(std::string name, TemplateType type, NodePtr content)
        : m_name(std::move(name)), m_type(type), m_content(std::move(content)) {}

    NodeType GetType() const override { return NodeType::TemplateDefinition; }

    const std::string& GetName() const { return m_name; }
    TemplateType GetTemplateType() const { return m_type; }
    const NodePtr& GetContent() const { return m_content; }

private:
    std::string m_name;
    TemplateType m_type;
    NodePtr m_content; // This will hold a StyleNode, ElementNode, etc.
};

} // namespace CHTL
