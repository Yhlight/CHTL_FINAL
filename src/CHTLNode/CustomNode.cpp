#include "CHTL/CHTLNode/CustomNode.h"
#include <sstream>

namespace CHTL {

CustomNode::CustomNode(CustomType type, const std::string& name)
    : BaseNode(NodeType::CUSTOM)
    , m_customType(type)
    , m_customName(name)
{
}

CustomNode::~CustomNode() {
}

void CustomNode::setCustomType(CustomType type) {
    m_customType = type;
}

CustomType CustomNode::getCustomType() const {
    return m_customType;
}

void CustomNode::setCustomName(const std::string& name) {
    m_customName = name;
}

const std::string& CustomNode::getCustomName() const {
    return m_customName;
}

void CustomNode::addSpecialization(const std::string& operation, const std::string& target) {
    m_specializations.emplace_back(operation, target);
}

const std::vector<std::pair<std::string, std::string>>& CustomNode::getSpecializations() const {
    return m_specializations;
}

void CustomNode::setParentTemplate(const std::string& parentTemplate) {
    m_parentTemplate = parentTemplate;
}

const std::string& CustomNode::getParentTemplate() const {
    return m_parentTemplate;
}

bool CustomNode::hasParentTemplate() const {
    return !m_parentTemplate.empty();
}

void CustomNode::setCustomContent(const std::string& content) {
    m_customContent = content;
}

const std::string& CustomNode::getCustomContent() const {
    return m_customContent;
}

std::shared_ptr<BaseNode> CustomNode::clone() const {
    auto cloned = std::make_shared<CustomNode>(m_customType, m_customName);
    cloned->m_specializations = m_specializations;
    cloned->m_parentTemplate = m_parentTemplate;
    cloned->m_customContent = m_customContent;
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

std::string CustomNode::toString() const {
    std::ostringstream oss;
    oss << "CustomNode(type=" << getCustomTypeName(m_customType)
        << ", name=\"" << m_customName << "\""
        << ", specializations=" << m_specializations.size()
        << ", parent=\"" << m_parentTemplate << "\""
        << ", children=" << m_children.size()
        << ", line=" << m_line << ", column=" << m_column << ")";
    return oss.str();
}

std::string CustomNode::getCustomTypeName(CustomType type) {
    switch (type) {
        case CustomType::STYLE:
            return "STYLE";
        case CustomType::ELEMENT:
            return "ELEMENT";
        case CustomType::VAR:
            return "VAR";
        case CustomType::UNKNOWN:
        default:
            return "UNKNOWN";
    }
}

} // namespace CHTL