#pragma once

#include "BaseNode.h"
#include <vector>
#include <memory>

namespace CHTL {

class StyleBlockNode : public BaseNode {
public:
    StyleBlockNode() = default;

    NodeType getType() const override { return NodeType::StyleBlock; }

    std::unique_ptr<BaseNode> clone() const override {
        auto newNode = std::make_unique<StyleBlockNode>();
        for (const auto& child : m_children) {
            newNode->addChild(child->clone());
        }
        return newNode;
    }

    void addChild(std::unique_ptr<BaseNode> child) {
        m_children.push_back(std::move(child));
    }

    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const { return m_children; }

private:
    std::vector<std::unique_ptr<BaseNode>> m_children;
};

} // namespace CHTL
