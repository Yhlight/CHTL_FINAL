#include "CHTL/CHTLNode/OperatorNode.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

OperatorNode::OperatorNode(OperatorType type) 
    : BaseNode(NodeType::OPERATOR), m_operatorType(type), m_insertPosition(InsertPosition::AFTER) {
}

OperatorNode::~OperatorNode() = default;

OperatorType OperatorNode::getOperatorType() const {
    return m_operatorType;
}

void OperatorNode::setTarget(const std::string& target) {
    m_target = target;
}

std::string OperatorNode::getTarget() const {
    return m_target;
}

bool OperatorNode::hasTarget() const {
    return !m_target.empty();
}

void OperatorNode::setInsertPosition(InsertPosition position) {
    m_insertPosition = position;
}

InsertPosition OperatorNode::getInsertPosition() const {
    return m_insertPosition;
}

void OperatorNode::setInsertContent(std::shared_ptr<BaseNode> content) {
    m_insertContent = content;
}

std::shared_ptr<BaseNode> OperatorNode::getInsertContent() const {
    return m_insertContent;
}

bool OperatorNode::hasInsertContent() const {
    return m_insertContent != nullptr;
}

void OperatorNode::setInheritTarget(const std::string& target) {
    m_inheritTarget = target;
}

std::string OperatorNode::getInheritTarget() const {
    return m_inheritTarget;
}

bool OperatorNode::hasInheritTarget() const {
    return !m_inheritTarget.empty();
}

void OperatorNode::setUseTarget(const std::string& target) {
    m_useTarget = target;
}

std::string OperatorNode::getUseTarget() const {
    return m_useTarget;
}

bool OperatorNode::hasUseTarget() const {
    return !m_useTarget.empty();
}

void OperatorNode::addDeleteTarget(const std::string& target) {
    m_deleteTargets.push_back(target);
}

std::vector<std::string> OperatorNode::getDeleteTargets() const {
    return m_deleteTargets;
}

bool OperatorNode::hasDeleteTarget(const std::string& target) const {
    return std::find(m_deleteTargets.begin(), m_deleteTargets.end(), target) != m_deleteTargets.end();
}

void OperatorNode::accept(NodeVisitor& visitor) {
    visitor.visitOperator(*this);
}

std::shared_ptr<BaseNode> OperatorNode::clone() const {
    auto cloned = std::make_shared<OperatorNode>(m_operatorType);
    cloned->m_target = m_target;
    cloned->m_insertPosition = m_insertPosition;
    cloned->m_insertContent = m_insertContent;
    cloned->m_inheritTarget = m_inheritTarget;
    cloned->m_useTarget = m_useTarget;
    cloned->m_deleteTargets = m_deleteTargets;
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

bool OperatorNode::validate() const {
    return BaseNode::validate();
}

std::string OperatorNode::toString() const {
    std::stringstream ss;
    ss << "OperatorNode{type=" << static_cast<int>(m_operatorType) 
       << ", target='" << m_target << "', insertPosition=" << static_cast<int>(m_insertPosition) 
       << ", insertContent=" << (m_insertContent ? "true" : "false") 
       << ", inheritTarget='" << m_inheritTarget << "', useTarget='" << m_useTarget 
       << "', deleteTargets=" << m_deleteTargets.size() << "}";
    return ss.str();
}

} // namespace CHTL