#include "ModificationNode.h"

namespace CHTL {

// --- InsertNode Implementation ---

InsertNode::InsertNode(InsertPosition position, const std::string& selector)
    : m_position(position), m_selector(selector) {}

NodeType InsertNode::getType() const {
    return NodeType::Insert;
}

std::unique_ptr<BaseNode> InsertNode::clone() const {
    auto new_node = std::make_unique<InsertNode>(m_position, m_selector);
    for (const auto& child : m_children) {
        new_node->addChild(child->clone());
    }
    return new_node;
}

InsertPosition InsertNode::getPosition() const {
    return m_position;
}

const std::string& InsertNode::getSelector() const {
    return m_selector;
}


// --- DeleteNode Implementation ---

DeleteNode::DeleteNode(const std::string& selector)
    : m_selector(selector) {}

NodeType DeleteNode::getType() const {
    return NodeType::Delete;
}

std::unique_ptr<BaseNode> DeleteNode::clone() const {
    return std::make_unique<DeleteNode>(m_selector);
}

const std::string& DeleteNode::getSelector() const {
    return m_selector;
}

} // namespace CHTL
