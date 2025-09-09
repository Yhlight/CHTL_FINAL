#include "CHTL/CHTLNode/BaseNode.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

BaseNode::BaseNode(NodeType type) 
    : m_type(type), m_line(0), m_column(0) {
}

BaseNode::~BaseNode() = default;

NodeType BaseNode::getType() const {
    return m_type;
}

void BaseNode::setAttribute(const std::string& key, const std::string& value) {
    m_attributes[key] = value;
}

std::string BaseNode::getAttribute(const std::string& key) const {
    auto it = m_attributes.find(key);
    return (it != m_attributes.end()) ? it->second : "";
}

bool BaseNode::hasAttribute(const std::string& key) const {
    return m_attributes.find(key) != m_attributes.end();
}

void BaseNode::removeAttribute(const std::string& key) {
    m_attributes.erase(key);
}

const std::unordered_map<std::string, std::string>& BaseNode::getAttributes() const {
    return m_attributes;
}

void BaseNode::addChild(std::shared_ptr<BaseNode> child) {
    if (child) {
        child->setParent(shared_from_this());
        m_children.push_back(child);
    }
}

void BaseNode::removeChild(std::shared_ptr<BaseNode> child) {
    auto it = std::find(m_children.begin(), m_children.end(), child);
    if (it != m_children.end()) {
        (*it)->setParent(nullptr);
        m_children.erase(it);
    }
}

const std::vector<std::shared_ptr<BaseNode>>& BaseNode::getChildren() const {
    return m_children;
}

void BaseNode::clearChildren() {
    for (auto& child : m_children) {
        child->setParent(nullptr);
    }
    m_children.clear();
}

void BaseNode::setParent(std::shared_ptr<BaseNode> parent) {
    m_parent = parent;
}

std::shared_ptr<BaseNode> BaseNode::getParent() const {
    return m_parent.lock();
}

void BaseNode::setContent(const std::string& content) {
    m_content = content;
}

std::string BaseNode::getContent() const {
    return m_content;
}

void BaseNode::setName(const std::string& name) {
    m_name = name;
}

std::string BaseNode::getName() const {
    return m_name;
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

bool BaseNode::validate() const {
    return true; // 基础验证，子类可以重写
}

std::string BaseNode::toString() const {
    std::stringstream ss;
    ss << "BaseNode{type=" << static_cast<int>(m_type) 
       << ", name='" << m_name << "', content='" << m_content << "'}";
    return ss.str();
}

} // namespace CHTL