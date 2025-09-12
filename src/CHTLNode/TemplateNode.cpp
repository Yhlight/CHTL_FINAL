#include "CHTL/CHTLNode/TemplateNode.h"
#include <sstream>

namespace CHTL {

TemplateNode::TemplateNode(TemplateType type, const std::string& name)
    : BaseNode(NodeType::TEMPLATE)
    , m_templateType(type)
    , m_templateName(name)
{
}

TemplateNode::~TemplateNode() {
}

void TemplateNode::setTemplateType(TemplateType type) {
    m_templateType = type;
}

TemplateType TemplateNode::getTemplateType() const {
    return m_templateType;
}

void TemplateNode::setTemplateName(const std::string& name) {
    m_templateName = name;
}

const std::string& TemplateNode::getTemplateName() const {
    return m_templateName;
}

void TemplateNode::addParameter(const std::string& paramName, const std::string& defaultValue) {
    m_parameters[paramName] = defaultValue;
}

std::string TemplateNode::getParameter(const std::string& paramName) const {
    auto it = m_parameters.find(paramName);
    if (it != m_parameters.end()) {
        return it->second;
    }
    return "";
}

bool TemplateNode::hasParameter(const std::string& paramName) const {
    return m_parameters.find(paramName) != m_parameters.end();
}

const std::unordered_map<std::string, std::string>& TemplateNode::getParameters() const {
    return m_parameters;
}

void TemplateNode::setParentTemplate(const std::string& parentTemplate) {
    m_parentTemplate = parentTemplate;
}

const std::string& TemplateNode::getParentTemplate() const {
    return m_parentTemplate;
}

bool TemplateNode::hasParentTemplate() const {
    return !m_parentTemplate.empty();
}

void TemplateNode::setTemplateContent(const std::string& content) {
    m_templateContent = content;
}

const std::string& TemplateNode::getTemplateContent() const {
    return m_templateContent;
}

std::shared_ptr<BaseNode> TemplateNode::clone() const {
    auto cloned = std::make_shared<TemplateNode>(m_templateType, m_templateName);
    cloned->m_parameters = m_parameters;
    cloned->m_parentTemplate = m_parentTemplate;
    cloned->m_templateContent = m_templateContent;
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

std::string TemplateNode::toString() const {
    std::ostringstream oss;
    oss << "TemplateNode(type=" << getTemplateTypeName(m_templateType)
        << ", name=\"" << m_templateName << "\""
        << ", parameters=" << m_parameters.size()
        << ", parent=\"" << m_parentTemplate << "\""
        << ", children=" << m_children.size()
        << ", line=" << m_line << ", column=" << m_column << ")";
    return oss.str();
}

std::string TemplateNode::getTemplateTypeName(TemplateType type) {
    switch (type) {
        case TemplateType::STYLE:
            return "STYLE";
        case TemplateType::ELEMENT:
            return "ELEMENT";
        case TemplateType::VAR:
            return "VAR";
        case TemplateType::UNKNOWN:
        default:
            return "UNKNOWN";
    }
}

} // namespace CHTL