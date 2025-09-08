#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

// Represents a `[Namespace]` block.
class NamespaceNode : public BaseNode {
public:
    explicit NamespaceNode(std::string name) : m_name(std::move(name)) {}

    NodeType getType() const override { return NodeType::Namespace; }

    std::unique_ptr<BaseNode> clone() const override {
        auto newNode = std::make_unique<NamespaceNode>(m_name);
        for (const auto& child : m_children) {
            newNode->addChild(child->clone());
        }
        return newNode;
    }

    const std::string& getName() const { return m_name; }

    void addChild(std::unique_ptr<BaseNode> child) {
        m_children.push_back(std::move(child));
    }

    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const {
        return m_children;
    }

private:
    std::string m_name;
    std::vector<std::unique_ptr<BaseNode>> m_children;
};

} // namespace CHTL
