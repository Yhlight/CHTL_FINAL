#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class CustomDefinitionNode : public BaseNode {
public:
    CustomDefinitionNode(std::string type, std::string name)
        : m_customType(std::move(type)), m_customName(std::move(name)) {}

    NodeType getType() const override { return NodeType::CustomDefinition; }

    std::unique_ptr<BaseNode> clone() const override {
        auto newNode = std::make_unique<CustomDefinitionNode>(m_customType, m_customName);
        for (const auto& child : m_children) {
            newNode->addChild(child->clone());
        }
        return newNode;
    }

    const std::string& getCustomType() const { return m_customType; }
    const std::string& getCustomName() const { return m_customName; }

    void addChild(std::unique_ptr<BaseNode> child) {
        m_children.push_back(std::move(child));
    }

    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const {
        return m_children;
    }

private:
    std::string m_customType;
    std::string m_customName;
    std::vector<std::unique_ptr<BaseNode>> m_children;
};

} // namespace CHTL
