#include "TemplateNode.h"

namespace CHTL {

TemplateNode::TemplateNode(TemplateType type, const std::string& name) 
    : BaseNode(NodeType::TEMPLATE), m_templateType(type), m_templateName(name) {
}

std::shared_ptr<BaseNode> TemplateNode::clone() const {
    auto cloned = std::make_shared<TemplateNode>(m_templateType, m_templateName);
    cloned->m_value = m_value;
    cloned->m_line = m_line;
    cloned->m_column = m_column;
    
    // 克隆子节点
    for (const auto& child : m_children) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

std::string TemplateNode::toString() const {
    std::string result = "[Template] ";
    
    switch (m_templateType) {
        case TemplateType::STYLE:
            result += "@Style " + m_templateName;
            break;
        case TemplateType::ELEMENT:
            result += "@Element " + m_templateName;
            break;
        case TemplateType::VAR:
            result += "@Var " + m_templateName;
            break;
    }
    
    result += " {\n";
    
    // 添加子节点内容
    for (const auto& child : m_children) {
        result += "    " + child->toString() + "\n";
    }
    
    result += "}";
    
    return result;
}

void TemplateNode::accept(NodeVisitor& visitor) {
    visitor.visitTemplate(*this);
}

} // namespace CHTL