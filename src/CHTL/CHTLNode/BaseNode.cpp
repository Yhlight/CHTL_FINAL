#include "BaseNode.h"
#include <sstream>

namespace CHTL {

BaseNode::BaseNode(NodeType type) 
    : m_type(type), m_line(0), m_column(0), m_parent(nullptr) {}

void BaseNode::addChild(std::unique_ptr<BaseNode> child) {
    if (child) {
        child->setParent(this);
        m_children.push_back(std::move(child));
    }
}

BaseNode* BaseNode::getChild(size_t index) const {
    if (index >= m_children.size()) {
        return nullptr;
    }
    return m_children[index].get();
}

std::unique_ptr<BaseNode> BaseNode::removeChild(size_t index) {
    if (index >= m_children.size()) {
        return nullptr;
    }
    
    auto child = std::move(m_children[index]);
    m_children.erase(m_children.begin() + index);
    child->setParent(nullptr);
    return child;
}

void BaseNode::clearChildren() {
    for (auto& child : m_children) {
        child->setParent(nullptr);
    }
    m_children.clear();
}

void BaseNode::setAttribute(const std::string& key, const std::any& value) {
    m_attributes[key] = value;
}

std::any BaseNode::getAttribute(const std::string& key) const {
    auto it = m_attributes.find(key);
    return it != m_attributes.end() ? it->second : std::any();
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

void BaseNode::setToken(std::unique_ptr<Token> token) {
    m_token = std::move(token);
}

size_t BaseNode::getDepth() const {
    size_t depth = 0;
    BaseNode* current = m_parent;
    while (current) {
        depth++;
        current = current->getParent();
    }
    return depth;
}

BaseNode* BaseNode::findChild(NodeType type) const {
    for (const auto& child : m_children) {
        if (child->getType() == type) {
            return child.get();
        }
    }
    return nullptr;
}

std::vector<BaseNode*> BaseNode::findAllChildren(NodeType type) const {
    std::vector<BaseNode*> result;
    for (const auto& child : m_children) {
        if (child->getType() == type) {
            result.push_back(child.get());
        }
    }
    return result;
}

std::string BaseNode::getPath() const {
    std::vector<std::string> path;
    BaseNode* current = const_cast<BaseNode*>(this);
    
    while (current && current->getParent()) {
        path.push_back(typeToString(current->getType()));
        current = current->getParent();
    }
    
    std::stringstream ss;
    for (auto it = path.rbegin(); it != path.rend(); ++it) {
        ss << *it;
        if (it + 1 != path.rend()) {
            ss << " -> ";
        }
    }
    
    return ss.str();
}

std::unique_ptr<BaseNode> BaseNode::clone() const {
    auto cloned = std::make_unique<BaseNode>(m_type);
    cloned->m_line = m_line;
    cloned->m_column = m_column;
    cloned->m_attributes = m_attributes;
    
    if (m_token) {
        cloned->m_token = std::make_unique<Token>(*m_token);
    }
    
    for (const auto& child : m_children) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

void BaseNode::accept(NodeVisitor* visitor) {
    if (visitor) {
        visitor->visit(this);
    }
}

std::string BaseNode::toString() const {
    std::stringstream ss;
    ss << typeToString(m_type);
    if (m_line > 0) {
        ss << " (行 " << m_line;
        if (m_column > 0) {
            ss << ", 列 " << m_column;
        }
        ss << ")";
    }
    return ss.str();
}

std::string BaseNode::typeToString(NodeType type) {
    switch (type) {
        case NodeType::ROOT: return "ROOT";
        case NodeType::ELEMENT: return "ELEMENT";
        case NodeType::TEXT: return "TEXT";
        case NodeType::COMMENT: return "COMMENT";
        case NodeType::STYLE: return "STYLE";
        case NodeType::SCRIPT: return "SCRIPT";
        case NodeType::TEMPLATE: return "TEMPLATE";
        case NodeType::CUSTOM: return "CUSTOM";
        case NodeType::ORIGIN: return "ORIGIN";
        case NodeType::IMPORT: return "IMPORT";
        case NodeType::CONFIGURATION: return "CONFIGURATION";
        case NodeType::NAMESPACE: return "NAMESPACE";
        case NodeType::DELETE: return "DELETE";
        case NodeType::INSERT: return "INSERT";
        case NodeType::INHERIT: return "INHERIT";
        case NodeType::ATTRIBUTE: return "ATTRIBUTE";
        case NodeType::STYLE_PROPERTY: return "STYLE_PROPERTY";
        case NodeType::CONDITIONAL: return "CONDITIONAL";
        case NodeType::LOGICAL_AND: return "LOGICAL_AND";
        case NodeType::LOGICAL_OR: return "LOGICAL_OR";
        case NodeType::COMPARISON: return "COMPARISON";
        case NodeType::VIR_OBJECT: return "VIR_OBJECT";
        case NodeType::LISTEN: return "LISTEN";
        case NodeType::ANIMATE: return "ANIMATE";
        case NodeType::ROUTER: return "ROUTER";
        case NodeType::FILELOADER: return "FILELOADER";
        case NodeType::DELEGATE: return "DELEGATE";
        case NodeType::INEVERAWAY: return "INEVERAWAY";
        case NodeType::UTIL_EXPR: return "UTIL_EXPR";
        case NodeType::PRINTMYLOVE: return "PRINTMYLOVE";
        default: return "UNKNOWN";
    }
}

} // namespace CHTL