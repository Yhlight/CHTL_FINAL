#include "CHTL/CHTLNode/TemplateNode.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

TemplateNode::TemplateNode(TemplateType type, const std::string& name) 
    : BaseNode(NodeType::TEMPLATE), m_templateType(type), m_templateName(name) {
}

TemplateNode::~TemplateNode() = default;

TemplateType TemplateNode::getTemplateType() const {
    return m_templateType;
}

void TemplateNode::setTemplateName(const std::string& name) {
    m_templateName = name;
}

std::string TemplateNode::getTemplateName() const {
    return m_templateName;
}

void TemplateNode::addInheritance(const std::string& templateName) {
    m_inheritances.push_back(templateName);
}

std::vector<std::string> TemplateNode::getInheritances() const {
    return m_inheritances;
}

bool TemplateNode::hasInheritance(const std::string& templateName) const {
    return std::find(m_inheritances.begin(), m_inheritances.end(), templateName) != m_inheritances.end();
}

void TemplateNode::setExplicitInheritance(const std::string& templateName) {
    m_explicitInheritance = templateName;
}

std::string TemplateNode::getExplicitInheritance() const {
    return m_explicitInheritance;
}

bool TemplateNode::hasExplicitInheritance() const {
    return !m_explicitInheritance.empty();
}

void TemplateNode::addVariable(const std::string& name, const std::string& value) {
    m_variables[name] = value;
}

std::string TemplateNode::getVariable(const std::string& name) const {
    auto it = m_variables.find(name);
    return (it != m_variables.end()) ? it->second : "";
}

bool TemplateNode::hasVariable(const std::string& name) const {
    return m_variables.find(name) != m_variables.end();
}

const std::unordered_map<std::string, std::string>& TemplateNode::getVariables() const {
    return m_variables;
}

void TemplateNode::addStyleProperty(const std::string& property, const std::string& value) {
    m_styleProperties[property] = value;
}

std::string TemplateNode::getStyleProperty(const std::string& property) const {
    auto it = m_styleProperties.find(property);
    return (it != m_styleProperties.end()) ? it->second : "";
}

bool TemplateNode::hasStyleProperty(const std::string& property) const {
    return m_styleProperties.find(property) != m_styleProperties.end();
}

const std::unordered_map<std::string, std::string>& TemplateNode::getStyleProperties() const {
    return m_styleProperties;
}

void TemplateNode::accept(NodeVisitor& visitor) {
    visitor.visitTemplate(*this);
}

std::shared_ptr<BaseNode> TemplateNode::clone() const {
    auto cloned = std::make_shared<TemplateNode>(m_templateType, m_templateName);
    cloned->m_inheritances = m_inheritances;
    cloned->m_explicitInheritance = m_explicitInheritance;
    cloned->m_variables = m_variables;
    cloned->m_styleProperties = m_styleProperties;
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

bool TemplateNode::validate() const {
    if (m_templateName.empty()) {
        return false;
    }
    
    return BaseNode::validate();
}

std::string TemplateNode::toString() const {
    std::stringstream ss;
    ss << "TemplateNode{type=" << static_cast<int>(m_templateType) 
       << ", name='" << m_templateName << "', inheritances=" << m_inheritances.size() 
       << ", variables=" << m_variables.size() << ", styleProperties=" << m_styleProperties.size() << "}";
    return ss.str();
}

} // namespace CHTL