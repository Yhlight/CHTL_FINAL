#include "CHTL/CHTLNode/NamespaceNode.h"
#include <sstream>

namespace CHTL {

NamespaceNode::NamespaceNode(const std::string& name)
    : BaseNode(NodeType::NAMESPACE)
    , m_namespaceName(name)
    , m_merge(false)
{
}

NamespaceNode::~NamespaceNode() {
}

void NamespaceNode::setNamespaceName(const std::string& name) {
    m_namespaceName = name;
}

const std::string& NamespaceNode::getNamespaceName() const {
    return m_namespaceName;
}

void NamespaceNode::setParentNamespace(const std::string& parent) {
    m_parentNamespace = parent;
}

const std::string& NamespaceNode::getParentNamespace() const {
    return m_parentNamespace;
}

bool NamespaceNode::hasParentNamespace() const {
    return !m_parentNamespace.empty();
}

void NamespaceNode::addChildNamespace(std::shared_ptr<NamespaceNode> child) {
    if (child) {
        m_childNamespaces.push_back(child);
    }
}

const std::vector<std::shared_ptr<NamespaceNode>>& NamespaceNode::getChildNamespaces() const {
    return m_childNamespaces;
}

void NamespaceNode::addSymbol(const std::string& name, const std::string& type, const std::string& value) {
    m_symbols[name] = std::make_pair(type, value);
}

std::string NamespaceNode::getSymbol(const std::string& name) const {
    auto it = m_symbols.find(name);
    if (it != m_symbols.end()) {
        return it->second.second;
    }
    return "";
}

bool NamespaceNode::hasSymbol(const std::string& name) const {
    return m_symbols.find(name) != m_symbols.end();
}

const std::unordered_map<std::string, std::pair<std::string, std::string>>& NamespaceNode::getSymbols() const {
    return m_symbols;
}

void NamespaceNode::setNamespaceContent(const std::string& content) {
    m_namespaceContent = content;
}

const std::string& NamespaceNode::getNamespaceContent() const {
    return m_namespaceContent;
}

void NamespaceNode::setMerge(bool merge) {
    m_merge = merge;
}

bool NamespaceNode::isMerge() const {
    return m_merge;
}

std::shared_ptr<BaseNode> NamespaceNode::clone() const {
    auto cloned = std::make_shared<NamespaceNode>(m_namespaceName);
    cloned->m_parentNamespace = m_parentNamespace;
    cloned->m_symbols = m_symbols;
    cloned->m_namespaceContent = m_namespaceContent;
    cloned->m_merge = m_merge;
    cloned->m_line = m_line;
    cloned->m_column = m_column;
    
    // 克隆子命名空间
    for (const auto& child : m_childNamespaces) {
        if (child) {
            cloned->addChildNamespace(std::dynamic_pointer_cast<NamespaceNode>(child->clone()));
        }
    }
    
    // 克隆子节点
    for (const auto& child : m_children) {
        if (child) {
            cloned->addChild(child->clone());
        }
    }
    
    return cloned;
}

std::string NamespaceNode::toString() const {
    std::ostringstream oss;
    oss << "NamespaceNode(name=\"" << m_namespaceName << "\""
        << ", parent=\"" << m_parentNamespace << "\""
        << ", symbols=" << m_symbols.size()
        << ", children=" << m_childNamespaces.size()
        << ", merge=" << (m_merge ? "true" : "false")
        << ", line=" << m_line << ", column=" << m_column << ")";
    return oss.str();
}

} // namespace CHTL