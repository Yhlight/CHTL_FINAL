#include "CHTL/CHTLNode/StyleNode.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

StyleNode::StyleNode(StyleType type) 
    : BaseNode(NodeType::STYLE), m_styleType(type) {
}

StyleNode::~StyleNode() = default;

StyleType StyleNode::getStyleType() const {
    return m_styleType;
}

void StyleNode::addProperty(const std::string& property, const std::string& value) {
    m_properties[property] = value;
}

std::string StyleNode::getProperty(const std::string& property) const {
    auto it = m_properties.find(property);
    return (it != m_properties.end()) ? it->second : "";
}

bool StyleNode::hasProperty(const std::string& property) const {
    return m_properties.find(property) != m_properties.end();
}

void StyleNode::removeProperty(const std::string& property) {
    m_properties.erase(property);
}

const std::unordered_map<std::string, std::string>& StyleNode::getProperties() const {
    return m_properties;
}

void StyleNode::addSelector(const std::string& selector) {
    m_selectors.push_back(selector);
}

std::vector<std::string> StyleNode::getSelectors() const {
    return m_selectors;
}

bool StyleNode::hasSelector(const std::string& selector) const {
    return std::find(m_selectors.begin(), m_selectors.end(), selector) != m_selectors.end();
}

void StyleNode::addClassSelector(const std::string& className) {
    m_classSelectors.push_back(className);
}

std::vector<std::string> StyleNode::getClassSelectors() const {
    return m_classSelectors;
}

void StyleNode::addIDSelector(const std::string& id) {
    m_idSelectors.push_back(id);
}

std::vector<std::string> StyleNode::getIDSelectors() const {
    return m_idSelectors;
}

void StyleNode::addPseudoClassSelector(const std::string& pseudoClass) {
    m_pseudoClassSelectors.push_back(pseudoClass);
}

std::vector<std::string> StyleNode::getPseudoClassSelectors() const {
    return m_pseudoClassSelectors;
}

void StyleNode::addPseudoElementSelector(const std::string& pseudoElement) {
    m_pseudoElementSelectors.push_back(pseudoElement);
}

std::vector<std::string> StyleNode::getPseudoElementSelectors() const {
    return m_pseudoElementSelectors;
}

void StyleNode::setContextReference(const std::string& reference) {
    m_contextReference = reference;
}

std::string StyleNode::getContextReference() const {
    return m_contextReference;
}

bool StyleNode::hasContextReference() const {
    return !m_contextReference.empty();
}

void StyleNode::addConditionalProperty(const std::string& property, const std::string& condition, 
                                     const std::string& trueValue, const std::string& falseValue) {
    m_conditionalProperties.push_back({property, condition, trueValue, falseValue});
}

std::vector<std::tuple<std::string, std::string, std::string, std::string>> StyleNode::getConditionalProperties() const {
    return m_conditionalProperties;
}

void StyleNode::addChainedCondition(const std::string& property, 
                                  const std::vector<std::pair<std::string, std::string>>& conditions) {
    m_chainedConditions.push_back({property, conditions});
}

std::vector<std::pair<std::string, std::vector<std::pair<std::string, std::string>>>> StyleNode::getChainedConditions() const {
    return m_chainedConditions;
}

void StyleNode::addReferenceCondition(const std::string& property, const std::string& reference, 
                                    const std::string& condition, const std::string& trueValue, 
                                    const std::string& falseValue) {
    m_referenceConditions.push_back({property, reference, condition, trueValue, falseValue});
}

std::vector<std::tuple<std::string, std::string, std::string, std::string, std::string>> StyleNode::getReferenceConditions() const {
    return m_referenceConditions;
}

void StyleNode::addTemplateReference(const std::string& templateName) {
    m_templateReferences.push_back(templateName);
}

std::vector<std::string> StyleNode::getTemplateReferences() const {
    return m_templateReferences;
}

void StyleNode::addCustomReference(const std::string& customName) {
    m_customReferences.push_back(customName);
}

std::vector<std::string> StyleNode::getCustomReferences() const {
    return m_customReferences;
}

void StyleNode::accept(NodeVisitor& visitor) {
    visitor.visitStyle(*this);
}

std::shared_ptr<BaseNode> StyleNode::clone() const {
    auto cloned = std::make_shared<StyleNode>(m_styleType);
    cloned->m_properties = m_properties;
    cloned->m_selectors = m_selectors;
    cloned->m_classSelectors = m_classSelectors;
    cloned->m_idSelectors = m_idSelectors;
    cloned->m_pseudoClassSelectors = m_pseudoClassSelectors;
    cloned->m_pseudoElementSelectors = m_pseudoElementSelectors;
    cloned->m_contextReference = m_contextReference;
    cloned->m_conditionalProperties = m_conditionalProperties;
    cloned->m_chainedConditions = m_chainedConditions;
    cloned->m_referenceConditions = m_referenceConditions;
    cloned->m_templateReferences = m_templateReferences;
    cloned->m_customReferences = m_customReferences;
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

bool StyleNode::validate() const {
    return BaseNode::validate();
}

std::string StyleNode::toString() const {
    std::stringstream ss;
    ss << "StyleNode{type=" << static_cast<int>(m_styleType) 
       << ", properties=" << m_properties.size() << ", selectors=" << m_selectors.size() 
       << ", classSelectors=" << m_classSelectors.size() << ", idSelectors=" << m_idSelectors.size() 
       << ", pseudoClassSelectors=" << m_pseudoClassSelectors.size() << ", pseudoElementSelectors=" << m_pseudoElementSelectors.size() 
       << ", conditionalProperties=" << m_conditionalProperties.size() << ", chainedConditions=" << m_chainedConditions.size() 
       << ", referenceConditions=" << m_referenceConditions.size() << ", templateReferences=" << m_templateReferences.size() 
       << ", customReferences=" << m_customReferences.size() << "}";
    return ss.str();
}

} // namespace CHTL