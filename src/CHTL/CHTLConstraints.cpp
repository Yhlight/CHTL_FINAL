#include "CHTL/CHTLConstraints.h"
#include <algorithm>
#include <sstream>

namespace CHTL {

// ConstraintRule 实现
ConstraintRule::ConstraintRule(ConstraintType type, const std::string& target, 
                               ConstraintTargetType targetType, int line, int column)
    : m_type(type), m_target(target), m_targetType(targetType), m_line(line), m_column(column) {
}

bool ConstraintRule::matches(const std::string& name, ConstraintTargetType type) const {
    switch (m_type) {
        case ConstraintType::EXACT:
            return isExactMatch(name);
        case ConstraintType::TYPE:
            return isTypeMatch(type);
        case ConstraintType::GLOBAL:
            return isExactMatch(name) || isTypeMatch(type);
        default:
            return false;
    }
}

bool ConstraintRule::isExactMatch(const std::string& name) const {
    return m_type == ConstraintType::EXACT && m_target == name;
}

bool ConstraintRule::isTypeMatch(ConstraintTargetType type) const {
    return m_type == ConstraintType::TYPE && m_targetType == type;
}

// ConstraintManager 实现
ConstraintManager::ConstraintManager(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
}

void ConstraintManager::addConstraint(std::shared_ptr<ConstraintRule> constraint) {
    if (constraint) {
        m_constraints.push_back(constraint);
        m_constraintsByType[constraint->getType()].push_back(constraint);
    }
}

void ConstraintManager::addConstraint(ConstraintType type, const std::string& target, 
                                     ConstraintTargetType targetType, int line, int column) {
    auto constraint = std::make_shared<ConstraintRule>(type, target, targetType, line, column);
    addConstraint(constraint);
}

bool ConstraintManager::isAllowed(const std::string& name, ConstraintTargetType type) const {
    // 检查精确约束
    if (!checkExactConstraints(name)) {
        return false;
    }
    
    // 检查类型约束
    if (!checkTypeConstraints(type)) {
        return false;
    }
    
    // 检查全局约束
    if (!checkGlobalConstraints(name, type)) {
        return false;
    }
    
    return true;
}

bool ConstraintManager::isExactAllowed(const std::string& name) const {
    return checkExactConstraints(name);
}

bool ConstraintManager::isTypeAllowed(ConstraintTargetType type) const {
    return checkTypeConstraints(type);
}

std::vector<std::shared_ptr<ConstraintRule>> ConstraintManager::getConstraints(ConstraintType type) const {
    auto it = m_constraintsByType.find(type);
    return (it != m_constraintsByType.end()) ? it->second : std::vector<std::shared_ptr<ConstraintRule>>();
}

std::vector<std::shared_ptr<ConstraintRule>> ConstraintManager::getAllConstraints() const {
    return m_constraints;
}

void ConstraintManager::applyConstraints(std::shared_ptr<BaseNode> root) {
    if (!root) {
        return;
    }
    
    // 递归应用约束到所有节点
    std::function<void(std::shared_ptr<BaseNode>)> applyToNode = 
        [&](std::shared_ptr<BaseNode> node) {
            applyConstraintsToNode(node);
            
            for (const auto& child : node->getChildren()) {
                applyToNode(child);
            }
        };
    
    applyToNode(root);
}

void ConstraintManager::applyConstraintsToNode(std::shared_ptr<BaseNode> node) {
    if (!node) {
        return;
    }
    
    // 根据节点类型应用相应的约束
    switch (node->getType()) {
        case NodeType::ELEMENT: {
            auto elementNode = std::static_pointer_cast<ElementNode>(node);
            if (!isAllowed(elementNode->getTagName(), ConstraintTargetType::HTML_ELEMENT)) {
                m_context->addError("Element not allowed: " + elementNode->getTagName(), 
                                  elementNode->getLine(), elementNode->getColumn());
            }
            break;
        }
        case NodeType::TEMPLATE_STYLE:
        case NodeType::TEMPLATE_ELEMENT:
        case NodeType::TEMPLATE_VAR: {
            auto templateNode = std::static_pointer_cast<TemplateNode>(node);
            if (!isAllowed(templateNode->getName(), ConstraintTargetType::TEMPLATE)) {
                m_context->addError("Template not allowed: " + templateNode->getName(), 
                                  templateNode->getLine(), templateNode->getColumn());
            }
            break;
        }
        case NodeType::CUSTOM_STYLE:
        case NodeType::CUSTOM_ELEMENT:
        case NodeType::CUSTOM_VAR: {
            auto customNode = std::static_pointer_cast<CustomNode>(node);
            if (!isAllowed(customNode->getName(), ConstraintTargetType::CUSTOM)) {
                m_context->addError("Custom not allowed: " + customNode->getName(), 
                                  customNode->getLine(), customNode->getColumn());
            }
            break;
        }
        case NodeType::ORIGIN: {
            auto originNode = std::static_pointer_cast<OriginNode>(node);
            if (!isAllowed(originNode->getName(), ConstraintTargetType::ORIGIN)) {
                m_context->addError("Origin not allowed: " + originNode->getName(), 
                                  originNode->getLine(), originNode->getColumn());
            }
            break;
        }
        default:
            break;
    }
}

void ConstraintManager::clear() {
    m_constraints.clear();
    m_constraintsByType.clear();
}

void ConstraintManager::removeConstraint(const std::string& target) {
    m_constraints.erase(
        std::remove_if(m_constraints.begin(), m_constraints.end(),
            [&target](const std::shared_ptr<ConstraintRule>& constraint) {
                return constraint->getTarget() == target;
            }),
        m_constraints.end()
    );
    
    // 同时从类型映射中移除
    for (auto& [type, constraints] : m_constraintsByType) {
        constraints.erase(
            std::remove_if(constraints.begin(), constraints.end(),
                [&target](const std::shared_ptr<ConstraintRule>& constraint) {
                    return constraint->getTarget() == target;
                }),
            constraints.end()
        );
    }
}

ConstraintTargetType ConstraintManager::parseTargetType(const std::string& typeString) const {
    if (typeString == "html" || typeString == "element") {
        return ConstraintTargetType::HTML_ELEMENT;
    } else if (typeString == "template") {
        return ConstraintTargetType::TEMPLATE;
    } else if (typeString == "custom") {
        return ConstraintTargetType::CUSTOM;
    } else if (typeString == "origin") {
        return ConstraintTargetType::ORIGIN;
    } else {
        return ConstraintTargetType::UNKNOWN;
    }
}

bool ConstraintManager::checkExactConstraints(const std::string& name) const {
    auto exactConstraints = getConstraints(ConstraintType::EXACT);
    for (const auto& constraint : exactConstraints) {
        if (constraint->isExactMatch(name)) {
            return false; // 被精确约束禁止
        }
    }
    return true;
}

bool ConstraintManager::checkTypeConstraints(ConstraintTargetType type) const {
    auto typeConstraints = getConstraints(ConstraintType::TYPE);
    for (const auto& constraint : typeConstraints) {
        if (constraint->isTypeMatch(type)) {
            return false; // 被类型约束禁止
        }
    }
    return true;
}

bool ConstraintManager::checkGlobalConstraints(const std::string& name, ConstraintTargetType type) const {
    auto globalConstraints = getConstraints(ConstraintType::GLOBAL);
    for (const auto& constraint : globalConstraints) {
        if (constraint->matches(name, type)) {
            return false; // 被全局约束禁止
        }
    }
    return true;
}

// ConstraintParser 实现
ConstraintParser::ConstraintParser(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
}

std::shared_ptr<ConstraintRule> ConstraintParser::parseConstraint(std::shared_ptr<BaseNode> node) {
    if (!node) {
        return nullptr;
    }
    
    // 这里需要根据具体的节点类型来解析约束
    // 暂时返回 nullptr
    return nullptr;
}

std::vector<std::shared_ptr<ConstraintRule>> ConstraintParser::parseConstraints(std::shared_ptr<BaseNode> root) {
    std::vector<std::shared_ptr<ConstraintRule>> constraints;
    
    if (!root) {
        return constraints;
    }
    
    // 递归查找约束节点
    std::function<void(std::shared_ptr<BaseNode>)> findConstraints = 
        [&](std::shared_ptr<BaseNode> node) {
            // 查找 except 语句
            if (node->getType() == NodeType::OPERATOR) {
                auto opNode = std::static_pointer_cast<OperatorNode>(node);
                if (opNode->getOperatorType() == OperatorNode::OperatorType::DELETE) {
                    // 解析 except 语句
                    auto exceptConstraints = parseExceptStatement(node);
                    constraints.insert(constraints.end(), exceptConstraints.begin(), exceptConstraints.end());
                }
            }
            
            for (const auto& child : node->getChildren()) {
                findConstraints(child);
            }
        };
    
    findConstraints(root);
    return constraints;
}

std::vector<std::shared_ptr<ConstraintRule>> ConstraintParser::parseExceptStatement(std::shared_ptr<BaseNode> node) {
    std::vector<std::shared_ptr<ConstraintRule>> constraints;
    
    // 解析 except 语句
    // 这里需要根据具体的语法规则来解析
    // 暂时返回空列表
    
    return constraints;
}

ConstraintType ConstraintParser::parseConstraintType(const std::string& typeString) const {
    if (typeString == "exact") {
        return ConstraintType::EXACT;
    } else if (typeString == "type") {
        return ConstraintType::TYPE;
    } else if (typeString == "global") {
        return ConstraintType::GLOBAL;
    } else {
        return ConstraintType::EXACT;
    }
}

ConstraintTargetType ConstraintParser::parseTargetType(const std::string& typeString) const {
    if (typeString == "html" || typeString == "element") {
        return ConstraintTargetType::HTML_ELEMENT;
    } else if (typeString == "template") {
        return ConstraintTargetType::TEMPLATE;
    } else if (typeString == "custom") {
        return ConstraintTargetType::CUSTOM;
    } else if (typeString == "origin") {
        return ConstraintTargetType::ORIGIN;
    } else {
        return ConstraintTargetType::UNKNOWN;
    }
}

std::vector<std::string> ConstraintParser::parseTargetList(const std::string& targetString) const {
    std::vector<std::string> targets;
    
    // 解析目标列表，支持逗号分隔
    std::stringstream ss(targetString);
    std::string target;
    
    while (std::getline(ss, target, ',')) {
        // 去除前后空格
        target.erase(0, target.find_first_not_of(" \t"));
        target.erase(target.find_last_not_of(" \t") + 1);
        
        if (!target.empty()) {
            targets.push_back(target);
        }
    }
    
    return targets;
}

// ConstraintValidator 实现
ConstraintValidator::ConstraintValidator(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
    m_constraintManager = std::make_unique<ConstraintManager>(context);
    m_parser = std::make_unique<ConstraintParser>(context);
}

bool ConstraintValidator::validateConstraints(std::shared_ptr<BaseNode> root) {
    if (!root) {
        return true;
    }
    
    // 解析约束
    auto constraints = m_parser->parseConstraints(root);
    for (const auto& constraint : constraints) {
        m_constraintManager->addConstraint(constraint);
    }
    
    // 验证约束
    return validateNode(root);
}

bool ConstraintValidator::validateNode(std::shared_ptr<BaseNode> node) {
    if (!node) {
        return true;
    }
    
    // 验证精确约束
    if (!validateExactConstraints(node)) {
        return false;
    }
    
    // 验证类型约束
    if (!validateTypeConstraints(node)) {
        return false;
    }
    
    // 验证全局约束
    if (!validateGlobalConstraints(node)) {
        return false;
    }
    
    // 递归验证子节点
    for (const auto& child : node->getChildren()) {
        if (!validateNode(child)) {
            return false;
        }
    }
    
    return true;
}

bool ConstraintValidator::validateExactConstraints(std::shared_ptr<BaseNode> node) {
    // 验证精确约束
    switch (node->getType()) {
        case NodeType::ELEMENT: {
            auto elementNode = std::static_pointer_cast<ElementNode>(node);
            return isElementAllowed(elementNode->getTagName());
        }
        case NodeType::TEMPLATE_STYLE:
        case NodeType::TEMPLATE_ELEMENT:
        case NodeType::TEMPLATE_VAR: {
            auto templateNode = std::static_pointer_cast<TemplateNode>(node);
            return isTemplateAllowed(templateNode->getName());
        }
        case NodeType::CUSTOM_STYLE:
        case NodeType::CUSTOM_ELEMENT:
        case NodeType::CUSTOM_VAR: {
            auto customNode = std::static_pointer_cast<CustomNode>(node);
            return isCustomAllowed(customNode->getName());
        }
        case NodeType::ORIGIN: {
            auto originNode = std::static_pointer_cast<OriginNode>(node);
            return isOriginAllowed(originNode->getName());
        }
        default:
            return true;
    }
}

bool ConstraintValidator::validateTypeConstraints(std::shared_ptr<BaseNode> node) {
    // 验证类型约束
    ConstraintTargetType targetType;
    
    switch (node->getType()) {
        case NodeType::ELEMENT:
            targetType = ConstraintTargetType::HTML_ELEMENT;
            break;
        case NodeType::TEMPLATE_STYLE:
        case NodeType::TEMPLATE_ELEMENT:
        case NodeType::TEMPLATE_VAR:
            targetType = ConstraintTargetType::TEMPLATE;
            break;
        case NodeType::CUSTOM_STYLE:
        case NodeType::CUSTOM_ELEMENT:
        case NodeType::CUSTOM_VAR:
            targetType = ConstraintTargetType::CUSTOM;
            break;
        case NodeType::ORIGIN:
            targetType = ConstraintTargetType::ORIGIN;
            break;
        default:
            return true;
    }
    
    return m_constraintManager->isTypeAllowed(targetType);
}

bool ConstraintValidator::validateGlobalConstraints(std::shared_ptr<BaseNode> node) {
    // 验证全局约束
    std::string name;
    ConstraintTargetType targetType;
    
    switch (node->getType()) {
        case NodeType::ELEMENT: {
            auto elementNode = std::static_pointer_cast<ElementNode>(node);
            name = elementNode->getTagName();
            targetType = ConstraintTargetType::HTML_ELEMENT;
            break;
        }
        case NodeType::TEMPLATE_STYLE:
        case NodeType::TEMPLATE_ELEMENT:
        case NodeType::TEMPLATE_VAR: {
            auto templateNode = std::static_pointer_cast<TemplateNode>(node);
            name = templateNode->getName();
            targetType = ConstraintTargetType::TEMPLATE;
            break;
        }
        case NodeType::CUSTOM_STYLE:
        case NodeType::CUSTOM_ELEMENT:
        case NodeType::CUSTOM_VAR: {
            auto customNode = std::static_pointer_cast<CustomNode>(node);
            name = customNode->getName();
            targetType = ConstraintTargetType::CUSTOM;
            break;
        }
        case NodeType::ORIGIN: {
            auto originNode = std::static_pointer_cast<OriginNode>(node);
            name = originNode->getName();
            targetType = ConstraintTargetType::ORIGIN;
            break;
        }
        default:
            return true;
    }
    
    return m_constraintManager->isAllowed(name, targetType);
}

void ConstraintValidator::reportConstraintViolation(const std::string& name, ConstraintTargetType type, 
                                                   const std::string& constraint, int line, int column) {
    std::string typeString;
    switch (type) {
        case ConstraintTargetType::HTML_ELEMENT:
            typeString = "HTML element";
            break;
        case ConstraintTargetType::TEMPLATE:
            typeString = "template";
            break;
        case ConstraintTargetType::CUSTOM:
            typeString = "custom";
            break;
        case ConstraintTargetType::ORIGIN:
            typeString = "origin";
            break;
        default:
            typeString = "unknown";
            break;
    }
    
    m_context->addError("Constraint violation: " + typeString + " '" + name + 
                       "' is not allowed by constraint '" + constraint + "'", line, column);
}

bool ConstraintValidator::isElementAllowed(const std::string& tagName) const {
    return m_constraintManager->isExactAllowed(tagName);
}

bool ConstraintValidator::isTemplateAllowed(const std::string& templateName) const {
    return m_constraintManager->isExactAllowed(templateName);
}

bool ConstraintValidator::isCustomAllowed(const std::string& customName) const {
    return m_constraintManager->isExactAllowed(customName);
}

bool ConstraintValidator::isOriginAllowed(const std::string& originName) const {
    return m_constraintManager->isExactAllowed(originName);
}

// ConstraintProcessor 实现
ConstraintProcessor::ConstraintProcessor(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
    m_constraintManager = std::make_unique<ConstraintManager>(context);
    m_parser = std::make_unique<ConstraintParser>(context);
    m_validator = std::make_unique<ConstraintValidator>(context);
}

void ConstraintProcessor::processConstraints(std::shared_ptr<BaseNode> root) {
    if (!root) {
        return;
    }
    
    // 解析约束
    auto constraints = m_parser->parseConstraints(root);
    for (const auto& constraint : constraints) {
        m_constraintManager->addConstraint(constraint);
    }
    
    // 应用约束
    applyConstraints(root);
}

void ConstraintProcessor::applyConstraints(std::shared_ptr<BaseNode> root) {
    if (!root) {
        return;
    }
    
    // 应用约束到所有节点
    m_constraintManager->applyConstraints(root);
}

bool ConstraintProcessor::validateConstraints(std::shared_ptr<BaseNode> root) {
    return m_validator->validateConstraints(root);
}

void ConstraintProcessor::processNodeConstraints(std::shared_ptr<BaseNode> node) {
    if (!node) {
        return;
    }
    
    // 根据节点类型处理约束
    switch (node->getType()) {
        case NodeType::ELEMENT:
            processElementConstraints(std::static_pointer_cast<ElementNode>(node));
            break;
        case NodeType::TEMPLATE_STYLE:
        case NodeType::TEMPLATE_ELEMENT:
        case NodeType::TEMPLATE_VAR:
            processTemplateConstraints(std::static_pointer_cast<TemplateNode>(node));
            break;
        case NodeType::CUSTOM_STYLE:
        case NodeType::CUSTOM_ELEMENT:
        case NodeType::CUSTOM_VAR:
            processCustomConstraints(std::static_pointer_cast<CustomNode>(node));
            break;
        case NodeType::ORIGIN:
            processOriginConstraints(std::static_pointer_cast<OriginNode>(node));
            break;
        default:
            break;
    }
}

void ConstraintProcessor::processElementConstraints(std::shared_ptr<ElementNode> node) {
    if (!node) {
        return;
    }
    
    // 处理元素约束
    if (!m_constraintManager->isAllowed(node->getTagName(), ConstraintTargetType::HTML_ELEMENT)) {
        m_context->addError("Element not allowed: " + node->getTagName(), 
                          node->getLine(), node->getColumn());
    }
}

void ConstraintProcessor::processTemplateConstraints(std::shared_ptr<TemplateNode> node) {
    if (!node) {
        return;
    }
    
    // 处理模板约束
    if (!m_constraintManager->isAllowed(node->getName(), ConstraintTargetType::TEMPLATE)) {
        m_context->addError("Template not allowed: " + node->getName(), 
                          node->getLine(), node->getColumn());
    }
}

void ConstraintProcessor::processCustomConstraints(std::shared_ptr<CustomNode> node) {
    if (!node) {
        return;
    }
    
    // 处理自定义约束
    if (!m_constraintManager->isAllowed(node->getName(), ConstraintTargetType::CUSTOM)) {
        m_context->addError("Custom not allowed: " + node->getName(), 
                          node->getLine(), node->getColumn());
    }
}

void ConstraintProcessor::processOriginConstraints(std::shared_ptr<OriginNode> node) {
    if (!node) {
        return;
    }
    
    // 处理原始嵌入约束
    if (!m_constraintManager->isAllowed(node->getName(), ConstraintTargetType::ORIGIN)) {
        m_context->addError("Origin not allowed: " + node->getName(), 
                          node->getLine(), node->getColumn());
    }
}

} // namespace CHTL