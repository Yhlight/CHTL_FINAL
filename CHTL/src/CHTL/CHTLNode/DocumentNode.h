#pragma once

#include "BaseNode.h"
#include <vector>
#include <memory>

namespace CHTL {

// Represents the root of the entire CHTL document.
// It acts as a container for all top-level nodes.
class DocumentNode : public BaseNode {
public:
    DocumentNode() = default;

    NodeType getType() const override { return NodeType::Document; }

    void addChild(std::unique_ptr<BaseNode> child) {
        m_children.push_back(std::move(child));
    }

    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const { return m_children; }

private:
    std::vector<std::unique_ptr<BaseNode>> m_children;
};

} // namespace CHTL
