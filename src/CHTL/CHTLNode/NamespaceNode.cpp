#include "CHTL/CHTLNode/NamespaceNode.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

NamespaceNode::NamespaceNode(const std::string& name) 
    : BaseNode(NodeType::NAMESPACE), m_namespaceName(name) {
}

NamespaceNode::~NamespaceNode() = default;

void NamespaceNode::setNamespaceName(const std::string& name) {
    m_namespaceName = name;
}

std::string NamespaceNode::getNamespaceName() const {
    return m_namespaceName;
}

void NamespaceNode::addNestedNamespace(const std::string& name, std::shared_ptr<NamespaceNode> namespaceNode) {
    m_nestedNamespaces[name] = namespaceNode;
}

std::shared_ptr<NamespaceNode> NamespaceNode::getNestedNamespace(const std::string& name) const {
    auto it = m_nestedNamespaces.find(name);
    return (it != m_nestedNamespaces.end()) ? it->second : nullptr;
}

bool NamespaceNode::hasNestedNamespace(const std::string& name) const {
    return m_nestedNamespaces.find(name) != m_nestedNamespaces.end();
}

const std::unordered_map<std::string, std::shared_ptr<NamespaceNode>>& NamespaceNode::getNestedNamespaces() const {
    return m_nestedNamespaces;
}

void NamespaceNode::addTemplate(const std::string& name, std::shared_ptr<TemplateNode> templateNode) {
    m_templates[name] = templateNode;
}

std::shared_ptr<TemplateNode> NamespaceNode::getTemplate(const std::string& name) const {
    auto it = m_templates.find(name);
    return (it != m_templates.end()) ? it->second : nullptr;
}

bool NamespaceNode::hasTemplate(const std::string& name) const {
    return m_templates.find(name) != m_templates.end();
}

const std::unordered_map<std::string, std::shared_ptr<TemplateNode>>& NamespaceNode::getTemplates() const {
    return m_templates;
}

void NamespaceNode::addCustom(const std::string& name, std::shared_ptr<CustomNode> customNode) {
    m_customs[name] = customNode;
}

std::shared_ptr<CustomNode> NamespaceNode::getCustom(const std::string& name) const {
    auto it = m_customs.find(name);
    return (it != m_customs.end()) ? it->second : nullptr;
}

bool NamespaceNode::hasCustom(const std::string& name) const {
    return m_customs.find(name) != m_customs.end();
}

const std::unordered_map<std::string, std::shared_ptr<CustomNode>>& NamespaceNode::getCustoms() const {
    return m_customs;
}

void NamespaceNode::addOrigin(const std::string& name, std::shared_ptr<OriginNode> originNode) {
    m_origins[name] = originNode;
}

std::shared_ptr<OriginNode> NamespaceNode::getOrigin(const std::string& name) const {
    auto it = m_origins.find(name);
    return (it != m_origins.end()) ? it->second : nullptr;
}

bool NamespaceNode::hasOrigin(const std::string& name) const {
    return m_origins.find(name) != m_origins.end();
}

const std::unordered_map<std::string, std::shared_ptr<OriginNode>>& NamespaceNode::getOrigins() const {
    return m_origins;
}

void NamespaceNode::addConfig(const std::string& name, std::shared_ptr<ConfigNode> configNode) {
    m_configs[name] = configNode;
}

std::shared_ptr<ConfigNode> NamespaceNode::getConfig(const std::string& name) const {
    auto it = m_configs.find(name);
    return (it != m_configs.end()) ? it->second : nullptr;
}

bool NamespaceNode::hasConfig(const std::string& name) const {
    return m_configs.find(name) != m_configs.end();
}

const std::unordered_map<std::string, std::shared_ptr<ConfigNode>>& NamespaceNode::getConfigs() const {
    return m_configs;
}

void NamespaceNode::addConstraint(const std::string& type, const std::string& target) {
    m_constraints.push_back({type, target});
}

std::vector<std::pair<std::string, std::string>> NamespaceNode::getConstraints() const {
    return m_constraints;
}

bool NamespaceNode::hasConstraint(const std::string& type, const std::string& target) const {
    return std::find(m_constraints.begin(), m_constraints.end(), std::make_pair(type, target)) != m_constraints.end();
}

void NamespaceNode::addGlobalConstraint(const std::string& type) {
    m_globalConstraints.push_back(type);
}

std::vector<std::string> NamespaceNode::getGlobalConstraints() const {
    return m_globalConstraints;
}

bool NamespaceNode::hasGlobalConstraint(const std::string& type) const {
    return std::find(m_globalConstraints.begin(), m_globalConstraints.end(), type) != m_globalConstraints.end();
}

void NamespaceNode::accept(NodeVisitor& visitor) {
    visitor.visitNamespace(*this);
}

std::shared_ptr<BaseNode> NamespaceNode::clone() const {
    auto cloned = std::make_shared<NamespaceNode>(m_namespaceName);
    cloned->m_nestedNamespaces = m_nestedNamespaces;
    cloned->m_templates = m_templates;
    cloned->m_customs = m_customs;
    cloned->m_origins = m_origins;
    cloned->m_configs = m_configs;
    cloned->m_constraints = m_constraints;
    cloned->m_globalConstraints = m_globalConstraints;
    cloned->m_content = m_content;
    cloned->m_name = m_name;
    cloned->m_line = m_line;
    cloned->m_column = m_column;
    cloned->m_attributes = m_attributes;
    
    // 克隆子节点
    for (const auto& child : m_children) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

bool NamespaceNode::validate() const {
    if (m_namespaceName.empty()) {
        return false;
    }
    
    return BaseNode::validate();
}

std::string NamespaceNode::toString() const {
    std::stringstream ss;
    ss << "NamespaceNode{name='" << m_namespaceName << "', nestedNamespaces=" << m_nestedNamespaces.size() 
       << ", templates=" << m_templates.size() << ", customs=" << m_customs.size() 
       << ", origins=" << m_origins.size() << ", configs=" << m_configs.size() 
       << ", constraints=" << m_constraints.size() << ", globalConstraints=" << m_globalConstraints.size() << "}";
    return ss.str();
}

} // namespace CHTL