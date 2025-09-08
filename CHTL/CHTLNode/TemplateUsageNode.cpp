#include "TemplateUsageNode.h"

namespace CHTL {

TemplateUsageNode::TemplateUsageNode(TemplateType templateType, const std::string& templateName)
    : m_templateType(templateType), m_templateName(templateName) {}

NodeType TemplateUsageNode::getType() const {
    return NodeType::TemplateUsage;
}

std::unique_ptr<BaseNode> TemplateUsageNode::clone() const {
    auto new_node = std::make_unique<TemplateUsageNode>(m_templateType, m_templateName);
    for (const auto& child : m_children) {
        new_node->addChild(child->clone());
    }
    return new_node;
}

TemplateType TemplateUsageNode::getTemplateType() const {
    return m_templateType;
}

const std::string& TemplateUsageNode::getTemplateName() const {
    return m_templateName;
}

} // namespace CHTL
