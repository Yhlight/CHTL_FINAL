#include "CHTL/CHTLNode/BaseNode.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

BaseNode::BaseNode(NodeType type)
    : m_type(type)
    , m_line(0)
    , m_column(0)
{
}

BaseNode::~BaseNode() {
}

NodeType BaseNode::getType() const {
    return m_type;
}

void BaseNode::setName(const std::string& name) {
    m_name = name;
}

const std::string& BaseNode::getName() const {
    return m_name;
}

void BaseNode::setValue(const std::string& value) {
    m_value = value;
}

const std::string& BaseNode::getValue() const {
    return m_value;
}

void BaseNode::addChild(std::shared_ptr<BaseNode> child) {
    if (child) {
        m_children.push_back(child);
    }
}

bool BaseNode::removeChild(size_t index) {
    if (index < m_children.size()) {
        m_children.erase(m_children.begin() + index);
        return true;
    }
    return false;
}

size_t BaseNode::getChildCount() const {
    return m_children.size();
}

std::shared_ptr<BaseNode> BaseNode::getChild(size_t index) const {
    if (index < m_children.size()) {
        return m_children[index];
    }
    return nullptr;
}

const std::vector<std::shared_ptr<BaseNode>>& BaseNode::getChildren() const {
    return m_children;
}

void BaseNode::setAttribute(const std::string& key, const std::string& value) {
    m_attributes[key] = value;
}

std::string BaseNode::getAttribute(const std::string& key, const std::string& defaultValue) const {
    auto it = m_attributes.find(key);
    if (it != m_attributes.end()) {
        return it->second;
    }
    return defaultValue;
}

bool BaseNode::hasAttribute(const std::string& key) const {
    return m_attributes.find(key) != m_attributes.end();
}

bool BaseNode::removeAttribute(const std::string& key) {
    auto it = m_attributes.find(key);
    if (it != m_attributes.end()) {
        m_attributes.erase(it);
        return true;
    }
    return false;
}

const std::unordered_map<std::string, std::string>& BaseNode::getAttributes() const {
    return m_attributes;
}

void BaseNode::setMetadata(const std::string& key, const std::any& value) {
    m_metadata[key] = value;
}

std::any BaseNode::getMetadata(const std::string& key) const {
    auto it = m_metadata.find(key);
    if (it != m_metadata.end()) {
        return it->second;
    }
    return std::any();
}

bool BaseNode::hasMetadata(const std::string& key) const {
    return m_metadata.find(key) != m_metadata.end();
}

void BaseNode::setPosition(size_t line, size_t column) {
    m_line = line;
    m_column = column;
}

size_t BaseNode::getLine() const {
    return m_line;
}

size_t BaseNode::getColumn() const {
    return m_column;
}

std::shared_ptr<BaseNode> BaseNode::clone() const {
    auto cloned = std::make_shared<BaseNode>(m_type);
    cloned->m_name = m_name;
    cloned->m_value = m_value;
    cloned->m_attributes = m_attributes;
    cloned->m_metadata = m_metadata;
    cloned->m_line = m_line;
    cloned->m_column = m_column;
    
    // 克隆子节点
    for (const auto& child : m_children) {
        if (child) {
            cloned->addChild(child->clone());
        }
    }
    
    return cloned;
}

std::string BaseNode::toString() const {
    std::ostringstream oss;
    oss << "BaseNode(type=" << static_cast<int>(m_type) 
        << ", name=\"" << m_name << "\""
        << ", value=\"" << m_value << "\""
        << ", children=" << m_children.size()
        << ", attributes=" << m_attributes.size()
        << ", line=" << m_line << ", column=" << m_column << ")";
    return oss.str();
}

// void BaseNode::accept(NodeVisitor& visitor) {
//     visitor.visit(*this);
// }

} // namespace CHTL