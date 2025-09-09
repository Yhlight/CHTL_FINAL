#include "TemplateNode.h"

namespace CHTL {

TemplateNode::TemplateNode(TemplateType type, const std::string& name)
    : BaseNode(NodeType::TEMPLATE), m_templateType(type), m_templateName(name) {}

void TemplateNode::addInheritance(const std::string& templateName) {
    // 避免重复添加
    if (std::find(m_inheritances.begin(), m_inheritances.end(), templateName) == m_inheritances.end()) {
        m_inheritances.push_back(templateName);
    }
}

bool TemplateNode::inheritsFrom(const std::string& templateName) const {
    return std::find(m_inheritances.begin(), m_inheritances.end(), templateName) != m_inheritances.end();
}

void TemplateNode::addProperty(const std::string& name, const std::string& value) {
    m_properties[name] = value;
}

std::string TemplateNode::getProperty(const std::string& name) const {
    auto it = m_properties.find(name);
    return it != m_properties.end() ? it->second : "";
}

bool TemplateNode::hasProperty(const std::string& name) const {
    return m_properties.find(name) != m_properties.end();
}

bool TemplateNode::removeProperty(const std::string& name) {
    auto it = m_properties.find(name);
    if (it != m_properties.end()) {
        m_properties.erase(it);
        return true;
    }
    return false;
}

std::unique_ptr<BaseNode> TemplateNode::clone() const {
    auto cloned = std::make_unique<TemplateNode>(m_templateType, m_templateName);
    cloned->m_inheritances = m_inheritances;
    cloned->m_properties = m_properties;
    cloned->m_line = m_line;
    cloned->m_column = m_column;
    
    if (m_token) {
        cloned->m_token = std::make_unique<Token>(*m_token);
    }
    
    for (const auto& child : m_children) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

void TemplateNode::accept(NodeVisitor* visitor) {
    if (visitor) {
        visitor->visitTemplate(this);
    }
}

std::string TemplateNode::toString() const {
    std::stringstream ss;
    ss << "TemplateNode(" << templateTypeToString(m_templateType) 
       << ", \"" << m_templateName << "\"";
    
    if (!m_inheritances.empty()) {
        ss << ", inherits: [";
        for (size_t i = 0; i < m_inheritances.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << m_inheritances[i];
        }
        ss << "]";
    }
    
    if (!m_properties.empty()) {
        ss << ", " << m_properties.size() << " properties";
    }
    
    ss << ")";
    return ss.str();
}

std::string TemplateNode::templateTypeToString(TemplateType type) {
    switch (type) {
        case TemplateType::STYLE: return "STYLE";
        case TemplateType::ELEMENT: return "ELEMENT";
        case TemplateType::VAR: return "VAR";
        default: return "UNKNOWN";
    }
}

TemplateType TemplateNode::parseTemplateType(const std::string& str) {
    if (str == "Style" || str == "style" || str == "@Style") {
        return TemplateType::STYLE;
    } else if (str == "Element" || str == "element" || str == "@Element") {
        return TemplateType::ELEMENT;
    } else if (str == "Var" || str == "var" || str == "@Var") {
        return TemplateType::VAR;
    }
    return TemplateType::STYLE; // 默认
}

} // namespace CHTL