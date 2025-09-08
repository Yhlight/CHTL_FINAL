#pragma once

#include "BaseNode.h"
#include <vector>
#include <memory>

namespace CHTL {

// Represents a `style { ... }` block within an element.
// It acts as a container for both direct style properties (which become inline styles)
// and selector blocks (which become global CSS rules).
class StyleBlockNode : public BaseNode {
public:
    StyleBlockNode() = default;

    NodeType getType() const override { return NodeType::StyleBlock; }

    void addChild(std::unique_ptr<BaseNode> child) {
        // Note: The parser will ensure children are either StylePropertyNode or StyleSelectorNode.
        m_children.push_back(std::move(child));
    }

    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const { return m_children; }

private:
    std::vector<std::unique_ptr<BaseNode>> m_children;
};

} // namespace CHTL
