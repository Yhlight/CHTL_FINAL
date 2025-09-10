#include "CHTLConstraintProcessor.h"
#include <algorithm>
#include <sstream>
#include <regex>
#include <cctype>

namespace CHTL {

CHTLConstraintProcessor::CHTLConstraintProcessor()
    : debugMode(false), strictMode(false), enableConstraints(true), enableValidation(true),
      enableWarnings(true), enableErrors(true), totalConstraints(0), activeConstraints(0),
      processedConstraints(0), violatedConstraints(0), errorConstraints(0), warningConstraints(0) {
    initializeConstraintTypes();
    initializeConstraintOperations();
    initializeConditionFunctions();
}

void CHTLConstraintProcessor::initializeConstraintTypes() {
    constraintTypeMap["exact"] = ConstraintNode::ConstraintType::EXACT;
    constraintTypeMap["type"] = ConstraintNode::ConstraintType::TYPE;
    constraintTypeMap["global"] = ConstraintNode::ConstraintType::GLOBAL;
    constraintTypeMap["local"] = ConstraintNode::ConstraintType::LOCAL;
    constraintTypeMap["inherited"] = ConstraintNode::ConstraintType::INHERITED;
    
    constraintTypeNames[ConstraintNode::ConstraintType::EXACT] = "exact";
    constraintTypeNames[ConstraintNode::ConstraintType::TYPE] = "type";
    constraintTypeNames[ConstraintNode::ConstraintType::GLOBAL] = "global";
    constraintTypeNames[ConstraintNode::ConstraintType::LOCAL] = "local";
    constraintTypeNames[ConstraintNode::ConstraintType::INHERITED] = "inherited";
}

void CHTLConstraintProcessor::initializeConstraintOperations() {
    constraintOperationMap["allow"] = ConstraintNode::ConstraintOperation::ALLOW;
    constraintOperationMap["deny"] = ConstraintNode::ConstraintOperation::DENY;
    constraintOperationMap["require"] = ConstraintNode::ConstraintOperation::REQUIRE;
    constraintOperationMap["optional"] = ConstraintNode::ConstraintOperation::OPTIONAL;
    constraintOperationMap["warn"] = ConstraintNode::ConstraintOperation::WARN;
    
    constraintOperationNames[ConstraintNode::ConstraintOperation::ALLOW] = "allow";
    constraintOperationNames[ConstraintNode::ConstraintOperation::DENY] = "deny";
    constraintOperationNames[ConstraintNode::ConstraintOperation::REQUIRE] = "require";
    constraintOperationNames[ConstraintNode::ConstraintOperation::OPTIONAL] = "optional";
    constraintOperationNames[ConstraintNode::ConstraintOperation::WARN] = "warn";
}

void CHTLConstraintProcessor::initializeConditionFunctions() {
    // 注册默认条件函数
    conditionFunctions["equals"] = [](const std::string& value) { return !value.empty(); };
    conditionFunctions["not_empty"] = [](const std::string& value) { return !value.empty(); };
    conditionFunctions["is_number"] = [](const std::string& value) { 
        return !value.empty() && std::all_of(value.begin(), value.end(), ::isdigit); 
    };
    conditionFunctions["is_alpha"] = [](const std::string& value) { 
        return !value.empty() && std::all_of(value.begin(), value.end(), ::isalpha); 
    };
    conditionFunctions["is_alnum"] = [](const std::string& value) { 
        return !value.empty() && std::all_of(value.begin(), value.end(), ::isalnum); 
    };
}

void CHTLConstraintProcessor::addConstraint(std::shared_ptr<ConstraintNode> constraint) {
    if (!constraint) return;
    
    constraints.push_back(constraint);
    constraintMap[constraint->getConstraintName()] = constraint;
    
    if (constraint->isActiveConstraint()) {
        activeConstraints++;
    }
    
    updateConstraintStatistics();
    updateConstraintMappings();
}

void CHTLConstraintProcessor::removeConstraint(const std::string& name) {
    auto it = constraintMap.find(name);
    if (it != constraintMap.end()) {
        auto constraint = it->second;
        constraintMap.erase(it);
        
        constraints.erase(std::remove(constraints.begin(), constraints.end(), constraint), constraints.end());
        
        if (constraint->isActiveConstraint()) {
            activeConstraints--;
        }
        
        updateConstraintStatistics();
        updateConstraintMappings();
    }
}

void CHTLConstraintProcessor::removeConstraint(std::shared_ptr<ConstraintNode> constraint) {
    if (!constraint) return;
    
    removeConstraint(constraint->getConstraintName());
}

std::shared_ptr<ConstraintNode> CHTLConstraintProcessor::getConstraint(const std::string& name) const {
    auto it = constraintMap.find(name);
    return (it != constraintMap.end()) ? it->second : nullptr;
}

std::vector<std::shared_ptr<ConstraintNode>> CHTLConstraintProcessor::getConstraints() const {
    return constraints;
}

std::vector<std::shared_ptr<ConstraintNode>> CHTLConstraintProcessor::getConstraintsByType(ConstraintNode::ConstraintType type) const {
    std::vector<std::shared_ptr<ConstraintNode>> result;
    for (const auto& constraint : constraints) {
        if (constraint->getConstraintType() == type) {
            result.push_back(constraint);
        }
    }
    return result;
}

std::vector<std::shared_ptr<ConstraintNode>> CHTLConstraintProcessor::getConstraintsByOperation(ConstraintNode::ConstraintOperation operation) const {
    std::vector<std::shared_ptr<ConstraintNode>> result;
    for (const auto& constraint : constraints) {
        if (constraint->getOperation() == operation) {
            result.push_back(constraint);
        }
    }
    return result;
}

void CHTLConstraintProcessor::addConstraintToGroup(const std::string& groupName, std::shared_ptr<ConstraintNode> constraint) {
    if (!constraint) return;
    
    constraintGroups[groupName].push_back(constraint);
}

void CHTLConstraintProcessor::removeConstraintFromGroup(const std::string& groupName, std::shared_ptr<ConstraintNode> constraint) {
    auto it = constraintGroups.find(groupName);
    if (it != constraintGroups.end()) {
        auto& group = it->second;
        group.erase(std::remove(group.begin(), group.end(), constraint), group.end());
    }
}

std::vector<std::shared_ptr<ConstraintNode>> CHTLConstraintProcessor::getConstraintsByGroup(const std::string& groupName) const {
    auto it = constraintGroups.find(groupName);
    return (it != constraintGroups.end()) ? it->second : std::vector<std::shared_ptr<ConstraintNode>>();
}

std::vector<std::string> CHTLConstraintProcessor::getConstraintGroups() const {
    std::vector<std::string> groups;
    for (const auto& pair : constraintGroups) {
        groups.push_back(pair.first);
    }
    return groups;
}

void CHTLConstraintProcessor::registerConstraintType(const std::string& name, ConstraintNode::ConstraintType type) {
    constraintTypeMap[name] = type;
    constraintTypeNames[type] = name;
}

void CHTLConstraintProcessor::unregisterConstraintType(const std::string& name) {
    auto it = constraintTypeMap.find(name);
    if (it != constraintTypeMap.end()) {
        constraintTypeNames.erase(it->second);
        constraintTypeMap.erase(it);
    }
}

ConstraintNode::ConstraintType CHTLConstraintProcessor::getConstraintType(const std::string& name) const {
    auto it = constraintTypeMap.find(name);
    return (it != constraintTypeMap.end()) ? it->second : ConstraintNode::ConstraintType::EXACT;
}

std::string CHTLConstraintProcessor::getConstraintTypeName(ConstraintNode::ConstraintType type) const {
    auto it = constraintTypeNames.find(type);
    return (it != constraintTypeNames.end()) ? it->second : "unknown";
}

std::vector<std::string> CHTLConstraintProcessor::getRegisteredConstraintTypes() const {
    std::vector<std::string> types;
    for (const auto& pair : constraintTypeMap) {
        types.push_back(pair.first);
    }
    return types;
}

void CHTLConstraintProcessor::registerConstraintOperation(const std::string& name, ConstraintNode::ConstraintOperation operation) {
    constraintOperationMap[name] = operation;
    constraintOperationNames[operation] = name;
}

void CHTLConstraintProcessor::unregisterConstraintOperation(const std::string& name) {
    auto it = constraintOperationMap.find(name);
    if (it != constraintOperationMap.end()) {
        constraintOperationNames.erase(it->second);
        constraintOperationMap.erase(it);
    }
}

ConstraintNode::ConstraintOperation CHTLConstraintProcessor::getConstraintOperation(const std::string& name) const {
    auto it = constraintOperationMap.find(name);
    return (it != constraintOperationMap.end()) ? it->second : ConstraintNode::ConstraintOperation::ALLOW;
}

std::string CHTLConstraintProcessor::getConstraintOperationName(ConstraintNode::ConstraintOperation operation) const {
    auto it = constraintOperationNames.find(operation);
    return (it != constraintOperationNames.end()) ? it->second : "unknown";
}

std::vector<std::string> CHTLConstraintProcessor::getRegisteredConstraintOperations() const {
    std::vector<std::string> operations;
    for (const auto& pair : constraintOperationMap) {
        operations.push_back(pair.first);
    }
    return operations;
}

void CHTLConstraintProcessor::addElementConstraint(const std::string& element, std::shared_ptr<ConstraintNode> constraint) {
    if (!constraint) return;
    
    elementConstraints[element].push_back(constraint);
}

void CHTLConstraintProcessor::removeElementConstraint(const std::string& element, std::shared_ptr<ConstraintNode> constraint) {
    auto it = elementConstraints.find(element);
    if (it != elementConstraints.end()) {
        auto& constraints = it->second;
        constraints.erase(std::remove(constraints.begin(), constraints.end(), constraint), constraints.end());
    }
}

std::vector<std::shared_ptr<ConstraintNode>> CHTLConstraintProcessor::getElementConstraints(const std::string& element) const {
    auto it = elementConstraints.find(element);
    return (it != elementConstraints.end()) ? it->second : std::vector<std::shared_ptr<ConstraintNode>>();
}

void CHTLConstraintProcessor::addPropertyConstraint(const std::string& property, std::shared_ptr<ConstraintNode> constraint) {
    if (!constraint) return;
    
    propertyConstraints[property].push_back(constraint);
}

void CHTLConstraintProcessor::removePropertyConstraint(const std::string& property, std::shared_ptr<ConstraintNode> constraint) {
    auto it = propertyConstraints.find(property);
    if (it != propertyConstraints.end()) {
        auto& constraints = it->second;
        constraints.erase(std::remove(constraints.begin(), constraints.end(), constraint), constraints.end());
    }
}

std::vector<std::shared_ptr<ConstraintNode>> CHTLConstraintProcessor::getPropertyConstraints(const std::string& property) const {
    auto it = propertyConstraints.find(property);
    return (it != propertyConstraints.end()) ? it->second : std::vector<std::shared_ptr<ConstraintNode>>();
}

void CHTLConstraintProcessor::addTypeConstraint(const std::string& type, std::shared_ptr<ConstraintNode> constraint) {
    if (!constraint) return;
    
    typeConstraints[type].push_back(constraint);
}

void CHTLConstraintProcessor::removeTypeConstraint(const std::string& type, std::shared_ptr<ConstraintNode> constraint) {
    auto it = typeConstraints.find(type);
    if (it != typeConstraints.end()) {
        auto& constraints = it->second;
        constraints.erase(std::remove(constraints.begin(), constraints.end(), constraint), constraints.end());
    }
}

std::vector<std::shared_ptr<ConstraintNode>> CHTLConstraintProcessor::getTypeConstraints(const std::string& type) const {
    auto it = typeConstraints.find(type);
    return (it != typeConstraints.end()) ? it->second : std::vector<std::shared_ptr<ConstraintNode>>();
}

void CHTLConstraintProcessor::registerConditionFunction(const std::string& name, std::function<bool(const std::string&)> function) {
    conditionFunctions[name] = function;
}

void CHTLConstraintProcessor::unregisterConditionFunction(const std::string& name) {
    conditionFunctions.erase(name);
}

std::function<bool(const std::string&)> CHTLConstraintProcessor::getConditionFunction(const std::string& name) const {
    auto it = conditionFunctions.find(name);
    return (it != conditionFunctions.end()) ? it->second : nullptr;
}

std::vector<std::string> CHTLConstraintProcessor::getRegisteredConditionFunctions() const {
    std::vector<std::string> functions;
    for (const auto& pair : conditionFunctions) {
        functions.push_back(pair.first);
    }
    return functions;
}

void CHTLConstraintProcessor::setConditionValue(const std::string& name, const std::string& value) {
    conditionValues[name] = value;
}

std::string CHTLConstraintProcessor::getConditionValue(const std::string& name) const {
    auto it = conditionValues.find(name);
    return (it != conditionValues.end()) ? it->second : "";
}

bool CHTLConstraintProcessor::hasConditionValue(const std::string& name) const {
    return conditionValues.find(name) != conditionValues.end();
}

void CHTLConstraintProcessor::removeConditionValue(const std::string& name) {
    conditionValues.erase(name);
}

bool CHTLConstraintProcessor::processConstraints() {
    if (!enableConstraints) {
        return true;
    }
    
    bool success = true;
    
    for (auto& constraint : constraints) {
        if (!processConstraint(constraint)) {
            success = false;
        }
    }
    
    return success;
}

bool CHTLConstraintProcessor::processConstraint(std::shared_ptr<ConstraintNode> constraint) {
    if (!constraint || !constraint->isActiveConstraint()) {
        return true;
    }
    
    try {
        bool result = constraint->processConstraint();
        if (result) {
            processedConstraints++;
        }
        return result;
    } catch (const std::exception& e) {
        handleConstraintError("处理约束时发生错误: " + std::string(e.what()));
        return false;
    }
}

bool CHTLConstraintProcessor::processConstraintGroup(const std::string& groupName) {
    auto groupConstraints = getConstraintsByGroup(groupName);
    bool success = true;
    
    for (auto& constraint : groupConstraints) {
        if (!processConstraint(constraint)) {
            success = false;
        }
    }
    
    return success;
}

bool CHTLConstraintProcessor::processConstraintType(ConstraintNode::ConstraintType type) {
    auto typeConstraints = getConstraintsByType(type);
    bool success = true;
    
    for (auto& constraint : typeConstraints) {
        if (!processConstraint(constraint)) {
            success = false;
        }
    }
    
    return success;
}

bool CHTLConstraintProcessor::processConstraintOperation(ConstraintNode::ConstraintOperation operation) {
    auto operationConstraints = getConstraintsByOperation(operation);
    bool success = true;
    
    for (auto& constraint : operationConstraints) {
        if (!processConstraint(constraint)) {
            success = false;
        }
    }
    
    return success;
}

bool CHTLConstraintProcessor::validateConstraints() const {
    if (!enableValidation) {
        return true;
    }
    
    bool success = true;
    
    for (const auto& constraint : constraints) {
        if (!validateConstraint(constraint)) {
            success = false;
        }
    }
    
    return success;
}

bool CHTLConstraintProcessor::validateConstraint(std::shared_ptr<ConstraintNode> constraint) const {
    if (!constraint) {
        return false;
    }
    
    return constraint->validateConstraint();
}

bool CHTLConstraintProcessor::validateConstraintGroup(const std::string& groupName) const {
    auto groupConstraints = getConstraintsByGroup(groupName);
    bool success = true;
    
    for (const auto& constraint : groupConstraints) {
        if (!validateConstraint(constraint)) {
            success = false;
        }
    }
    
    return success;
}

bool CHTLConstraintProcessor::validateConstraintType(ConstraintNode::ConstraintType type) const {
    auto typeConstraints = getConstraintsByType(type);
    bool success = true;
    
    for (const auto& constraint : typeConstraints) {
        if (!validateConstraint(constraint)) {
            success = false;
        }
    }
    
    return success;
}

bool CHTLConstraintProcessor::validateConstraintOperation(ConstraintNode::ConstraintOperation operation) const {
    auto operationConstraints = getConstraintsByOperation(operation);
    bool success = true;
    
    for (const auto& constraint : operationConstraints) {
        if (!validateConstraint(constraint)) {
            success = false;
        }
    }
    
    return success;
}

bool CHTLConstraintProcessor::checkConstraints(const std::string& value) const {
    if (!enableConstraints) {
        return true;
    }
    
    bool success = true;
    
    for (const auto& constraint : constraints) {
        if (!constraint->checkConstraint(value)) {
            success = false;
        }
    }
    
    return success;
}

bool CHTLConstraintProcessor::checkElementConstraints(const std::string& element, const std::string& value) const {
    auto elementConstraints = getElementConstraints(element);
    bool success = true;
    
    for (const auto& constraint : elementConstraints) {
        if (!constraint->checkConstraint(value)) {
            success = false;
        }
    }
    
    return success;
}

bool CHTLConstraintProcessor::checkPropertyConstraints(const std::string& property, const std::string& value) const {
    auto propertyConstraints = getPropertyConstraints(property);
    bool success = true;
    
    for (const auto& constraint : propertyConstraints) {
        if (!constraint->checkConstraint(value)) {
            success = false;
        }
    }
    
    return success;
}

bool CHTLConstraintProcessor::checkTypeConstraints(const std::string& type, const std::string& value) const {
    auto typeConstraints = getTypeConstraints(type);
    bool success = true;
    
    for (const auto& constraint : typeConstraints) {
        if (!constraint->checkConstraint(value)) {
            success = false;
        }
    }
    
    return success;
}

bool CHTLConstraintProcessor::checkGlobalConstraints(const std::string& value) const {
    return checkConstraints(value);
}

bool CHTLConstraintProcessor::checkLocalConstraints(const std::string& value) const {
    return checkConstraints(value);
}

bool CHTLConstraintProcessor::checkInheritedConstraints(const std::string& value) const {
    return checkConstraints(value);
}

bool CHTLConstraintProcessor::applyConstraints(std::shared_ptr<BaseNode> node) {
    if (!node || !enableConstraints) {
        return true;
    }
    
    bool success = true;
    
    // 应用全局约束
    if (!applyGlobalConstraints(node)) {
        success = false;
    }
    
    // 应用类型约束
    if (!applyTypeConstraints(node, "ELEMENT")) {
        success = false;
    }
    
    return success;
}

bool CHTLConstraintProcessor::applyGlobalConstraints(std::shared_ptr<BaseNode> node) {
    if (!node || !enableConstraints) {
        return true;
    }
    // 简化实现：使用全局检查
    return checkGlobalConstraints(node->getValue());
}

bool CHTLConstraintProcessor::applyElementConstraints(std::shared_ptr<ElementNode> element) {
    if (!element || !enableConstraints) {
        return true;
    }
    
    bool success = true;
    
    // 应用元素约束
    if (!applyExactConstraints(element)) {
        success = false;
    }
    
    // 应用属性约束
    for (const auto& attr : element->getAttributes()) {
        if (!applyPropertyConstraints(element, attr.first, attr.second)) {
            success = false;
        }
    }
    
    return success;
}

bool CHTLConstraintProcessor::applyPropertyConstraints(std::shared_ptr<ElementNode> element, const std::string& property, const std::string& value) {
    if (!element || !enableConstraints) {
        return true;
    }
    
    return checkPropertyConstraints(property, value);
}

bool CHTLConstraintProcessor::applyTypeConstraints(std::shared_ptr<BaseNode> node, const std::string& type) {
    if (!node || !enableConstraints) {
        return true;
    }
    
    return checkTypeConstraints(type, node->getValue());
}

void CHTLConstraintProcessor::updateConstraintStatistics() {
    totalConstraints = constraints.size();
    activeConstraints = 0;
    processedConstraints = 0;
    violatedConstraints = 0;
    errorConstraints = 0;
    warningConstraints = 0;
    
    for (const auto& constraint : constraints) {
        if (constraint->isActiveConstraint()) {
            activeConstraints++;
        }
        if (constraint->isProcessedConstraint()) {
            processedConstraints++;
        }
        if (constraint->isViolatedConstraint()) {
            violatedConstraints++;
        }
        if (!constraint->getConstraintErrors().empty()) {
            errorConstraints++;
        }
        if (!constraint->getConstraintWarnings().empty()) {
            warningConstraints++;
        }
    }
}

void CHTLConstraintProcessor::updateConstraintMappings() {
    // 更新约束映射
    elementConstraints.clear();
    propertyConstraints.clear();
    typeConstraints.clear();
    
    for (const auto& constraint : constraints) {
        // 更新元素约束映射
        for (const auto& element : constraint->getTargetElements()) {
            elementConstraints[element].push_back(constraint);
        }
        
        // 更新属性约束映射
        for (const auto& property : constraint->getTargetProperties()) {
            propertyConstraints[property].push_back(constraint);
        }
        
        // 更新类型约束映射
        for (const auto& type : constraint->getTargetTypes()) {
            typeConstraints[type].push_back(constraint);
        }
    }
}

void CHTLConstraintProcessor::updateConstraintPriorities() {
    priorityConstraints.clear();
    
    for (const auto& constraint : constraints) {
        int priority = constraint->getPriority();
        priorityConstraints[priority].push_back(constraint);
    }
}

void CHTLConstraintProcessor::addValidationError(const std::string& error) {
    validationErrors.push_back(error);
}

void CHTLConstraintProcessor::addValidationWarning(const std::string& warning) {
    validationWarnings.push_back(warning);
}

void CHTLConstraintProcessor::addConstraintViolation(const std::string& violation) {
    constraintViolations.push_back(violation);
}

void CHTLConstraintProcessor::clearValidationMessages() {
    validationErrors.clear();
    validationWarnings.clear();
}

void CHTLConstraintProcessor::clearConstraintViolations() {
    constraintViolations.clear();
}

void CHTLConstraintProcessor::reset() {
    resetConstraints();
    resetConstraintGroups();
    resetConstraintTypes();
    resetConstraintOperations();
    resetConstraintTargets();
    resetConstraintConditions();
    resetConstraintConfig();
    resetConstraintPriorities();
}

void CHTLConstraintProcessor::resetConstraints() {
    constraints.clear();
    constraintMap.clear();
    updateConstraintStatistics();
}

void CHTLConstraintProcessor::resetConstraintGroups() {
    constraintGroups.clear();
}

void CHTLConstraintProcessor::resetConstraintTypes() {
    constraintTypeMap.clear();
    constraintTypeNames.clear();
    initializeConstraintTypes();
}

void CHTLConstraintProcessor::resetConstraintOperations() {
    constraintOperationMap.clear();
    constraintOperationNames.clear();
    initializeConstraintOperations();
}

void CHTLConstraintProcessor::resetConstraintTargets() {
    elementConstraints.clear();
    propertyConstraints.clear();
    typeConstraints.clear();
}

void CHTLConstraintProcessor::resetConstraintConditions() {
    conditionFunctions.clear();
    conditionValues.clear();
    initializeConditionFunctions();
}

void CHTLConstraintProcessor::resetConstraintConfig() {
    constraintConfig.clear();
    constraintFlags.clear();
    constraintLimits.clear();
}

void CHTLConstraintProcessor::resetConstraintPriorities() {
    constraintPriorities.clear();
    priorityConstraints.clear();
}

void CHTLConstraintProcessor::clearConstraints() {
    constraints.clear();
    constraintMap.clear();
    updateConstraintStatistics();
}

void CHTLConstraintProcessor::clearConstraintGroups() {
    constraintGroups.clear();
}

void CHTLConstraintProcessor::clearConstraintTypes() {
    constraintTypeMap.clear();
    constraintTypeNames.clear();
}

void CHTLConstraintProcessor::clearConstraintOperations() {
    constraintOperationMap.clear();
    constraintOperationNames.clear();
}

void CHTLConstraintProcessor::clearConstraintTargets() {
    elementConstraints.clear();
    propertyConstraints.clear();
    typeConstraints.clear();
}

void CHTLConstraintProcessor::clearConstraintConditions() {
    conditionFunctions.clear();
    conditionValues.clear();
}

void CHTLConstraintProcessor::clearConstraintConfig() {
    constraintConfig.clear();
    constraintFlags.clear();
    constraintLimits.clear();
}

void CHTLConstraintProcessor::clearConstraintPriorities() {
    constraintPriorities.clear();
    priorityConstraints.clear();
}

void CHTLConstraintProcessor::optimizeConstraints() {
    for (auto& constraint : constraints) {
        optimizeConstraint(constraint);
    }
}

void CHTLConstraintProcessor::optimizeConstraint(std::shared_ptr<ConstraintNode> constraint) {
    if (!constraint) return;
    
    // 优化约束
    constraint->processConstraint();
}

void CHTLConstraintProcessor::optimizeConstraintGroup(const std::string& groupName) {
    auto groupConstraints = getConstraintsByGroup(groupName);
    for (auto& constraint : groupConstraints) {
        optimizeConstraint(constraint);
    }
}

void CHTLConstraintProcessor::optimizeConstraintType(ConstraintNode::ConstraintType type) {
    auto typeConstraints = getConstraintsByType(type);
    for (auto& constraint : typeConstraints) {
        optimizeConstraint(constraint);
    }
}

void CHTLConstraintProcessor::optimizeConstraintOperation(ConstraintNode::ConstraintOperation operation) {
    auto operationConstraints = getConstraintsByOperation(operation);
    for (auto& constraint : operationConstraints) {
        optimizeConstraint(constraint);
    }
}

std::string CHTLConstraintProcessor::getDebugInfo() const {
    std::ostringstream oss;
    oss << "CHTLConstraintProcessor Debug Info:\n";
    oss << "  Total Constraints: " << totalConstraints << "\n";
    oss << "  Active Constraints: " << activeConstraints << "\n";
    oss << "  Processed Constraints: " << processedConstraints << "\n";
    oss << "  Violated Constraints: " << violatedConstraints << "\n";
    oss << "  Error Constraints: " << errorConstraints << "\n";
    oss << "  Warning Constraints: " << warningConstraints << "\n";
    oss << "  Debug Mode: " << (debugMode ? "true" : "false") << "\n";
    oss << "  Strict Mode: " << (strictMode ? "true" : "false") << "\n";
    oss << "  Constraints Enabled: " << (enableConstraints ? "true" : "false") << "\n";
    oss << "  Validation Enabled: " << (enableValidation ? "true" : "false") << "\n";
    oss << "  Warnings Enabled: " << (enableWarnings ? "true" : "false") << "\n";
    oss << "  Errors Enabled: " << (enableErrors ? "true" : "false") << "\n";
    
    if (!validationErrors.empty()) {
        oss << "  Validation Errors: ";
        for (size_t i = 0; i < validationErrors.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << validationErrors[i];
        }
        oss << "\n";
    }
    
    if (!validationWarnings.empty()) {
        oss << "  Validation Warnings: ";
        for (size_t i = 0; i < validationWarnings.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << validationWarnings[i];
        }
        oss << "\n";
    }
    
    if (!constraintViolations.empty()) {
        oss << "  Constraint Violations: ";
        for (size_t i = 0; i < constraintViolations.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << constraintViolations[i];
        }
        oss << "\n";
    }
    
    return oss.str();
}

void CHTLConstraintProcessor::handleConstraintError(const std::string& error) {
    addValidationError(error);
    if (enableErrors) {
        // 处理约束错误
    }
}

void CHTLConstraintProcessor::handleConstraintWarning(const std::string& warning) {
    addValidationWarning(warning);
    if (enableWarnings) {
        // 处理约束警告
    }
}

void CHTLConstraintProcessor::handleConstraintViolation(const std::string& violation) {
    addConstraintViolation(violation);
    // 处理约束违反
}

// 私有辅助方法实现
bool CHTLConstraintProcessor::processExactConstraints() {
    return processConstraintType(ConstraintNode::ConstraintType::EXACT);
}

bool CHTLConstraintProcessor::processTypeConstraints() {
    return processConstraintType(ConstraintNode::ConstraintType::TYPE);
}

bool CHTLConstraintProcessor::processGlobalConstraints() {
    return processConstraintType(ConstraintNode::ConstraintType::GLOBAL);
}

bool CHTLConstraintProcessor::processLocalConstraints() {
    return processConstraintType(ConstraintNode::ConstraintType::LOCAL);
}

bool CHTLConstraintProcessor::processInheritedConstraints() {
    return processConstraintType(ConstraintNode::ConstraintType::INHERITED);
}

bool CHTLConstraintProcessor::validateExactConstraints() const {
    return validateConstraintType(ConstraintNode::ConstraintType::EXACT);
}

bool CHTLConstraintProcessor::validateTypeConstraints() const {
    return validateConstraintType(ConstraintNode::ConstraintType::TYPE);
}

bool CHTLConstraintProcessor::validateGlobalConstraints() const {
    return validateConstraintType(ConstraintNode::ConstraintType::GLOBAL);
}

bool CHTLConstraintProcessor::validateLocalConstraints() const {
    return validateConstraintType(ConstraintNode::ConstraintType::LOCAL);
}

bool CHTLConstraintProcessor::validateInheritedConstraints() const {
    return validateConstraintType(ConstraintNode::ConstraintType::INHERITED);
}

bool CHTLConstraintProcessor::checkExactConstraints(const std::string& value) const {
    auto exactConstraints = getConstraintsByType(ConstraintNode::ConstraintType::EXACT);
    bool success = true;
    
    for (const auto& constraint : exactConstraints) {
        if (!constraint->checkConstraint(value)) {
            success = false;
        }
    }
    
    return success;
}


bool CHTLConstraintProcessor::applyExactConstraints(std::shared_ptr<BaseNode> node) {
    return checkExactConstraints(node->getValue());
}


} // namespace CHTL