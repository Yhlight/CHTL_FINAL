#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>

namespace CHTL {

class NamespaceNode : public BaseNode {
public:
    NamespaceNode(std::string name, NodeList content)
        : m_name(std::move(name)), m_content(std::move(content)) {}

    NodeType GetType() const override { return NodeType::Namespace; }

    const std::string& GetName() const { return m_name; }
    const NodeList& GetContent() const { return m_content; }

    NodePtr Clone() const override {
        NodeList clonedContent;
        for (const auto& node : m_content) {
            clonedContent.push_back(node->Clone());
        }
        return std::make_shared<NamespaceNode>(m_name, clonedContent);
    }

private:
    std::string m_name;
    NodeList m_content;
};

} // namespace CHTL
