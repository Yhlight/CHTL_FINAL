#pragma once

#include "BaseNode.h"
#include <string>
#include <memory>
#include <vector>

namespace CHTL {

enum class TemplateType {
    Style,
    Element,
    Var
};

class TemplateDefinitionNode : public BaseNode {
public:
    TemplateDefinitionNode(std::string name, TemplateType type, NodeList content, bool isCustom)
        : m_name(std::move(name)), m_type(type), m_content(std::move(content)), m_isCustom(isCustom) {}

    NodeType GetType() const override { return NodeType::TemplateDefinition; }

    const std::string& GetName() const { return m_name; }
    TemplateType GetTemplateType() const { return m_type; }
    const NodeList& GetContent() const { return m_content; }
    bool IsCustom() const { return m_isCustom; }

    NodePtr Clone() const override {
        NodeList clonedContent;
        for (const auto& node : m_content) {
            clonedContent.push_back(node->Clone());
        }
        return std::make_shared<TemplateDefinitionNode>(m_name, m_type, clonedContent, m_isCustom);
    }

private:
    std::string m_name;
    TemplateType m_type;
    NodeList m_content;
    bool m_isCustom;
};

} // namespace CHTL
