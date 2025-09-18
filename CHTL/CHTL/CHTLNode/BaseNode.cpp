#include "BaseNode.h"
#include <algorithm>

namespace CHTL {

BaseNode::BaseNode(NodeType type) 
    : m_type(type), m_line(0), m_column(0) {
}

const std::string& BaseNode::getAttribute(const std::string& name) const {
    static const std::string empty;
    auto it = m_attributes.find(name);
    return (it != m_attributes.end()) ? it->second : empty;
}

void BaseNode::setAttribute(const std::string& name, const std::string& value) {
    m_attributes[name] = value;
}

bool BaseNode::hasAttribute(const std::string& name) const {
    return m_attributes.find(name) != m_attributes.end();
}

void BaseNode::removeAttribute(const std::string& name) {
    m_attributes.erase(name);
}

void BaseNode::addChild(std::shared_ptr<BaseNode> child) {
    if (child) {
        m_children.push_back(child);
    }
}

void BaseNode::removeChild(std::shared_ptr<BaseNode> child) {
    auto it = std::find(m_children.begin(), m_children.end(), child);
    if (it != m_children.end()) {
        m_children.erase(it);
    }
}

std::vector<std::shared_ptr<BaseNode>> BaseNode::findChildren(NodeType type) const {
    std::vector<std::shared_ptr<BaseNode>> result;
    for (const auto& child : m_children) {
        if (child->getType() == type) {
            result.push_back(child);
        }
    }
    return result;
}

std::shared_ptr<BaseNode> BaseNode::findFirstChild(NodeType type) const {
    for (const auto& child : m_children) {
        if (child->getType() == type) {
            return child;
        }
    }
    return nullptr;
}

} // namespace CHTL