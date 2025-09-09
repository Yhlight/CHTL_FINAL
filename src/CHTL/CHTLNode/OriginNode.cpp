#include "CHTL/CHTLNode/OriginNode.h"
#include <sstream>

namespace CHTL {

OriginNode::OriginNode(OriginType type, const std::string& name) 
    : BaseNode(NodeType::ORIGIN), m_originType(type), m_originName(name) {
}

OriginNode::~OriginNode() = default;

OriginType OriginNode::getOriginType() const {
    return m_originType;
}

void OriginNode::setOriginContent(const std::string& content) {
    m_originContent = content;
}

std::string OriginNode::getOriginContent() const {
    return m_originContent;
}

void OriginNode::setOriginName(const std::string& name) {
    m_originName = name;
}

std::string OriginNode::getOriginName() const {
    return m_originName;
}

bool OriginNode::hasOriginName() const {
    return !m_originName.empty();
}

void OriginNode::setCustomTypeName(const std::string& typeName) {
    m_customTypeName = typeName;
}

std::string OriginNode::getCustomTypeName() const {
    return m_customTypeName;
}

bool OriginNode::hasCustomTypeName() const {
    return !m_customTypeName.empty();
}

void OriginNode::accept(NodeVisitor& visitor) {
    visitor.visitOrigin(*this);
}

std::shared_ptr<BaseNode> OriginNode::clone() const {
    auto cloned = std::make_shared<OriginNode>(m_originType, m_originName);
    cloned->m_originContent = m_originContent;
    cloned->m_customTypeName = m_customTypeName;
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

bool OriginNode::validate() const {
    return BaseNode::validate();
}

std::string OriginNode::toString() const {
    std::stringstream ss;
    ss << "OriginNode{type=" << static_cast<int>(m_originType) 
       << ", name='" << m_originName << "', content='" << m_originContent 
       << "', customTypeName='" << m_customTypeName << "'}";
    return ss.str();
}

} // namespace CHTL