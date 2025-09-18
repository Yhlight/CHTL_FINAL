#include "CustomNode.h"

namespace CHTL {

CustomNode::CustomNode(CustomType type, const std::string& name) 
    : BaseNode(NodeType::CUSTOM), m_customType(type), m_customName(name) {
}

std::shared_ptr<BaseNode> CustomNode::clone() const {
    auto cloned = std::make_shared<CustomNode>(m_customType, m_customName);
    cloned->m_value = m_value;
    cloned->m_line = m_line;
    cloned->m_column = m_column;
    
    // 克隆子节点
    for (const auto& child : m_children) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

std::string CustomNode::toString() const {
    std::string result = "[Custom] ";
    
    switch (m_customType) {
        case CustomType::STYLE:
            result += "@Style " + m_customName;
            break;
        case CustomType::ELEMENT:
            result += "@Element " + m_customName;
            break;
        case CustomType::VAR:
            result += "@Var " + m_customName;
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

void CustomNode::accept(NodeVisitor& visitor) {
    visitor.visitCustom(*this);
}

} // namespace CHTL