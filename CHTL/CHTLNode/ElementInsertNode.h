#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

enum class InsertPosition {
    After,
    Before,
    Replace,
    AtTop,
    AtBottom
};

// Represents an instruction to insert elements within a [Custom] @Element usage.
// e.g., `insert after div[0] { ... }`
class ElementInsertNode : public BaseNode {
public:
    ElementInsertNode(InsertPosition position, std::string targetSelector, NodeList nodesToInsert)
        : m_position(position), m_targetSelector(std::move(targetSelector)), m_nodes_to_insert(std::move(nodesToInsert)) {}

    NodeType GetType() const override { return NodeType::ElementInsertion; }

    InsertPosition GetPosition() const { return m_position; }
    const std::string& GetTargetSelector() const { return m_targetSelector; }
    const NodeList& GetNodesToInsert() const { return m_nodes_to_insert; }

    NodePtr Clone() const override {
        NodeList clonedNodes;
        for (const auto& node : m_nodes_to_insert) {
            clonedNodes.push_back(node->Clone());
        }
        return std::make_shared<ElementInsertNode>(m_position, m_targetSelector, clonedNodes);
    }

private:
    InsertPosition m_position;
    std::string m_targetSelector; // Can be empty for AtTop/AtBottom
    NodeList m_nodes_to_insert;
};

using ElementInsertNodePtr = std::shared_ptr<ElementInsertNode>;

} // namespace CHTL
