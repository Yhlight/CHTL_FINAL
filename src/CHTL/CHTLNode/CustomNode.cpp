#include "CHTL/CHTLNode/CustomNode.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

CustomNode::CustomNode(CustomType type, const std::string& name) 
    : BaseNode(NodeType::CUSTOM), m_customType(type), m_customName(name) {
}

CustomNode::~CustomNode() = default;

CustomType CustomNode::getCustomType() const {
    return m_customType;
}

void CustomNode::setCustomName(const std::string& name) {
    m_customName = name;
}

std::string CustomNode::getCustomName() const {
    return m_customName;
}

void CustomNode::addSpecialization(const std::string& operation, const std::string& target) {
    m_specializations.push_back({operation, target});
}

std::vector<std::pair<std::string, std::string>> CustomNode::getSpecializations() const {
    return m_specializations;
}

void CustomNode::addDeleteOperation(const std::string& target) {
    m_deleteOperations.push_back(target);
}

std::vector<std::string> CustomNode::getDeleteOperations() const {
    return m_deleteOperations;
}

void CustomNode::addInsertOperation(const std::string& position, const std::string& target, 
                                  std::shared_ptr<BaseNode> content) {
    m_insertOperations.push_back({position, target, content});
}

std::vector<std::tuple<std::string, std::string, std::shared_ptr<BaseNode>>> CustomNode::getInsertOperations() const {
    return m_insertOperations;
}

void CustomNode::addUnvaluedProperty(const std::string& property) {
    m_unvaluedProperties.push_back(property);
}

std::vector<std::string> CustomNode::getUnvaluedProperties() const {
    return m_unvaluedProperties;
}

bool CustomNode::hasUnvaluedProperty(const std::string& property) const {
    return std::find(m_unvaluedProperties.begin(), m_unvaluedProperties.end(), property) != m_unvaluedProperties.end();
}

void CustomNode::addVariableSpecialization(const std::string& name, const std::string& value) {
    m_variableSpecializations[name] = value;
}

std::string CustomNode::getVariableSpecialization(const std::string& name) const {
    auto it = m_variableSpecializations.find(name);
    return (it != m_variableSpecializations.end()) ? it->second : "";
}

bool CustomNode::hasVariableSpecialization(const std::string& name) const {
    return m_variableSpecializations.find(name) != m_variableSpecializations.end();
}

const std::unordered_map<std::string, std::string>& CustomNode::getVariableSpecializations() const {
    return m_variableSpecializations;
}

void CustomNode::setFullQualifiedName(const std::string& name) {
    m_fullQualifiedName = name;
}

std::string CustomNode::getFullQualifiedName() const {
    return m_fullQualifiedName;
}

bool CustomNode::hasFullQualifiedName() const {
    return !m_fullQualifiedName.empty();
}

void CustomNode::accept(NodeVisitor& visitor) {
    visitor.visitCustom(*this);
}

std::shared_ptr<BaseNode> CustomNode::clone() const {
    auto cloned = std::make_shared<CustomNode>(m_customType, m_customName);
    cloned->m_fullQualifiedName = m_fullQualifiedName;
    cloned->m_specializations = m_specializations;
    cloned->m_deleteOperations = m_deleteOperations;
    cloned->m_insertOperations = m_insertOperations;
    cloned->m_unvaluedProperties = m_unvaluedProperties;
    cloned->m_variableSpecializations = m_variableSpecializations;
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

bool CustomNode::validate() const {
    if (m_customName.empty()) {
        return false;
    }
    
    return BaseNode::validate();
}

std::string CustomNode::toString() const {
    std::stringstream ss;
    ss << "CustomNode{type=" << static_cast<int>(m_customType) 
       << ", name='" << m_customName << "', specializations=" << m_specializations.size() 
       << ", deleteOperations=" << m_deleteOperations.size() << ", insertOperations=" << m_insertOperations.size() 
       << ", unvaluedProperties=" << m_unvaluedProperties.size() << ", variableSpecializations=" << m_variableSpecializations.size() << "}";
    return ss.str();
}

} // namespace CHTL