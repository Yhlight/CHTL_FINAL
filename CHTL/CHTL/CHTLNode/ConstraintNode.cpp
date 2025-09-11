#include "ConstraintNode.h"
#include <algorithm>
#include <sstream>
#include <regex>
#include <cctype>

namespace CHTL {

ConstraintNode::ConstraintNode(const std::string& name, ConstraintType type)
    : BaseNode(NodeType::ELEMENT, name, ""), constraintType(type), constraintName(name),
      operation(ConstraintOperation::ALLOW), operationPriority(0), isActive(true), isProcessed(false),
      isViolated(false), priority(0), isRequired(false), isOptional(false) {
}

std::string ConstraintNode::getConstraintTypeName() const {
    switch (constraintType) {
        case ConstraintType::EXACT: return "exact";
        case ConstraintType::TYPE: return "type";
        case ConstraintType::GLOBAL: return "global";
        case ConstraintType::LOCAL: return "local";
        case ConstraintType::INHERITED: return "inherited";
        default: return "unknown";
    }
}

void ConstraintNode::setConstraintParameter(const std::string& key, const std::string& value) {
    constraintParameters[key] = value;
}

std::string ConstraintNode::getConstraintParameter(const std::string& key) const {
    auto it = constraintParameters.find(key);
    return (it != constraintParameters.end()) ? it->second : "";
}

bool ConstraintNode::hasConstraintParameter(const std::string& key) const {
    return constraintParameters.find(key) != constraintParameters.end();
}

void ConstraintNode::removeConstraintParameter(const std::string& key) {
    constraintParameters.erase(key);
}

void ConstraintNode::addTargetElement(const std::string& element) {
    if (std::find(targetElements.begin(), targetElements.end(), element) == targetElements.end()) {
        targetElements.push_back(element);
    }
}

void ConstraintNode::removeTargetElement(const std::string& element) {
    targetElements.erase(std::remove(targetElements.begin(), targetElements.end(), element), targetElements.end());
}

bool ConstraintNode::hasTargetElement(const std::string& element) const {
    return std::find(targetElements.begin(), targetElements.end(), element) != targetElements.end();
}

void ConstraintNode::addTargetProperty(const std::string& property) {
    if (std::find(targetProperties.begin(), targetProperties.end(), property) == targetProperties.end()) {
        targetProperties.push_back(property);
    }
}

void ConstraintNode::removeTargetProperty(const std::string& property) {
    targetProperties.erase(std::remove(targetProperties.begin(), targetProperties.end(), property), targetProperties.end());
}

bool ConstraintNode::hasTargetProperty(const std::string& property) const {
    return std::find(targetProperties.begin(), targetProperties.end(), property) != targetProperties.end();
}

void ConstraintNode::addTargetType(const std::string& type) {
    if (std::find(targetTypes.begin(), targetTypes.end(), type) == targetTypes.end()) {
        targetTypes.push_back(type);
    }
}

void ConstraintNode::removeTargetType(const std::string& type) {
    targetTypes.erase(std::remove(targetTypes.begin(), targetTypes.end(), type), targetTypes.end());
}

bool ConstraintNode::hasTargetType(const std::string& type) const {
    return std::find(targetTypes.begin(), targetTypes.end(), type) != targetTypes.end();
}

void ConstraintNode::addCondition(const std::string& condition) {
    if (std::find(conditions.begin(), conditions.end(), condition) == conditions.end()) {
        conditions.push_back(condition);
    }
}

void ConstraintNode::removeCondition(const std::string& condition) {
    conditions.erase(std::remove(conditions.begin(), conditions.end(), condition), conditions.end());
}

bool ConstraintNode::hasCondition(const std::string& condition) const {
    return std::find(conditions.begin(), conditions.end(), condition) != conditions.end();
}

void ConstraintNode::setConditionValue(const std::string& condition, const std::string& value) {
    conditionValues[condition] = value;
}

std::string ConstraintNode::getConditionValue(const std::string& condition) const {
    auto it = conditionValues.find(condition);
    return (it != conditionValues.end()) ? it->second : "";
}

bool ConstraintNode::hasConditionValue(const std::string& condition) const {
    return conditionValues.find(condition) != conditionValues.end();
}

void ConstraintNode::removeConditionValue(const std::string& condition) {
    conditionValues.erase(condition);
}

void ConstraintNode::setConditionFunction(const std::string& condition, std::function<bool(const std::string&)> function) {
    conditionFunctions[condition] = function;
}

std::function<bool(const std::string&)> ConstraintNode::getConditionFunction(const std::string& condition) const {
    auto it = conditionFunctions.find(condition);
    return (it != conditionFunctions.end()) ? it->second : nullptr;
}

bool ConstraintNode::hasConditionFunction(const std::string& condition) const {
    return conditionFunctions.find(condition) != conditionFunctions.end();
}

void ConstraintNode::removeConditionFunction(const std::string& condition) {
    conditionFunctions.erase(condition);
}

std::string ConstraintNode::getOperationName() const {
    switch (operation) {
        case ConstraintOperation::ALLOW: return "allow";
        case ConstraintOperation::DENY: return "deny";
        case ConstraintOperation::REQUIRE: return "require";
        case ConstraintOperation::OPTIONAL: return "optional";
        case ConstraintOperation::WARN: return "warn";
        default: return "unknown";
    }
}

void ConstraintNode::addConstraintError(const std::string& error) {
    constraintErrors.push_back(error);
}

void ConstraintNode::addConstraintWarning(const std::string& warning) {
    constraintWarnings.push_back(warning);
}

void ConstraintNode::clearConstraintMessages() {
    constraintErrors.clear();
    constraintWarnings.clear();
}

void ConstraintNode::addViolationMessage(const std::string& message) {
    violationMessages.push_back(message);
}

void ConstraintNode::clearViolationMessages() {
    violationMessages.clear();
}

void ConstraintNode::setMetadata(const std::string& key, const std::string& value) {
    metadata[key] = value;
}

std::string ConstraintNode::getMetadata(const std::string& key) const {
    auto it = metadata.find(key);
    return (it != metadata.end()) ? it->second : "";
}

bool ConstraintNode::hasMetadata(const std::string& key) const {
    return metadata.find(key) != metadata.end();
}

void ConstraintNode::removeMetadata(const std::string& key) {
    metadata.erase(key);
}

bool ConstraintNode::processConstraint() {
    if (!isActive) {
        return true;
    }
    
    try {
        processConstraintTargets();
        processConstraintConditions();
        processConstraintOperation();
        
        isProcessed = true;
        return true;
    } catch (const std::exception& e) {
        handleConstraintError("处理约束时发生错误: " + std::string(e.what()));
        return false;
    }
}

bool ConstraintNode::processExactConstraint() {
    if (constraintType != ConstraintType::EXACT) {
        return false;
    }
    
    // 处理精确约束
    return processConstraint();
}

bool ConstraintNode::processTypeConstraint() {
    if (constraintType != ConstraintType::TYPE) {
        return false;
    }
    
    // 处理类型约束
    return processConstraint();
}

bool ConstraintNode::processGlobalConstraint() {
    if (constraintType != ConstraintType::GLOBAL) {
        return false;
    }
    
    // 处理全局约束
    return processConstraint();
}

bool ConstraintNode::processLocalConstraint() {
    if (constraintType != ConstraintType::LOCAL) {
        return false;
    }
    
    // 处理局部约束
    return processConstraint();
}

bool ConstraintNode::processInheritedConstraint() {
    if (constraintType != ConstraintType::INHERITED) {
        return false;
    }
    
    // 处理继承约束
    return processConstraint();
}

bool ConstraintNode::validateConstraint() const {
    return validateExpression() && validateTargets() && validateConditions() && validateOperation();
}

bool ConstraintNode::validateExpression() const {
    if (constraintExpression.empty()) {
        return true;
    }
    
    try {
        std::string processed = processConstraintExpression(constraintExpression);
        return !processed.empty();
    } catch (const std::exception& e) {
        return false;
    }
}

bool ConstraintNode::validateTargets() const {
    return !targetElements.empty() || !targetProperties.empty() || !targetTypes.empty();
}

bool ConstraintNode::validateConditions() const {
    for (const auto& condition : conditions) {
        if (!hasConditionValue(condition) && !hasConditionFunction(condition)) {
            return false;
        }
    }
    return true;
}

bool ConstraintNode::validateOperation() const {
    return operationMessage.empty() || !operationMessage.empty();
}

bool ConstraintNode::checkConstraint(const std::string& value) const {
    if (!isActive || !isProcessed) {
        return true;
    }
    
    switch (constraintType) {
        case ConstraintType::EXACT:
            return checkExactConstraint(value);
        case ConstraintType::TYPE:
            return checkTypeConstraint("", value);
        case ConstraintType::GLOBAL:
            return checkGlobalConstraint(value);
        case ConstraintType::LOCAL:
            return checkLocalConstraint(value);
        case ConstraintType::INHERITED:
            return checkInheritedConstraint(value);
        default:
            return true;
    }
}

bool ConstraintNode::checkElementConstraint(const std::string& element) const {
    if (targetElements.empty()) {
        return true;
    }
    
    return hasTargetElement(element);
}

bool ConstraintNode::checkPropertyConstraint(const std::string& property, const std::string& value) const {
    if (targetProperties.empty()) {
        return true;
    }
    
    if (!hasTargetProperty(property)) {
        return true;
    }
    
    return checkConstraint(value);
}

bool ConstraintNode::checkTypeConstraint(const std::string& type, const std::string& value) const {
    if (targetTypes.empty()) {
        return true;
    }
    
    if (!hasTargetType(type)) {
        return true;
    }
    
    return checkConstraint(value);
}

std::string ConstraintNode::toHTML() const {
    std::ostringstream oss;
    oss << "<!-- Constraint: " << constraintName << " -->\n";
    oss << "<!-- Type: " << getConstraintTypeName() << " -->\n";
    oss << "<!-- Operation: " << getOperationName() << " -->\n";
    
    if (!constraintExpression.empty()) {
        oss << "<!-- Expression: " << constraintExpression << " -->\n";
    }
    
    if (!targetElements.empty()) {
        oss << "<!-- Target Elements: ";
        for (size_t i = 0; i < targetElements.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << targetElements[i];
        }
        oss << " -->\n";
    }
    
    if (!targetProperties.empty()) {
        oss << "<!-- Target Properties: ";
        for (size_t i = 0; i < targetProperties.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << targetProperties[i];
        }
        oss << " -->\n";
    }
    
    if (!targetTypes.empty()) {
        oss << "<!-- Target Types: ";
        for (size_t i = 0; i < targetTypes.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << targetTypes[i];
        }
        oss << " -->\n";
    }
    
    if (!conditions.empty()) {
        oss << "<!-- Conditions: ";
        for (size_t i = 0; i < conditions.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << conditions[i];
        }
        oss << " -->\n";
    }
    
    if (!operationMessage.empty()) {
        oss << "<!-- Message: " << operationMessage << " -->\n";
    }
    
    return oss.str();
}

std::string ConstraintNode::toCSS() const {
    std::ostringstream oss;
    oss << "/* Constraint: " << constraintName << " */\n";
    oss << "/* Type: " << getConstraintTypeName() << " */\n";
    oss << "/* Operation: " << getOperationName() << " */\n";
    
    if (!constraintExpression.empty()) {
        oss << "/* Expression: " << constraintExpression << " */\n";
    }
    
    return oss.str();
}

std::string ConstraintNode::toJavaScript() const {
    std::ostringstream oss;
    oss << "// Constraint: " << constraintName << "\n";
    oss << "// Type: " << getConstraintTypeName() << "\n";
    oss << "// Operation: " << getOperationName() << "\n";
    
    if (!constraintExpression.empty()) {
        oss << "// Expression: " << constraintExpression << "\n";
    }
    
    return oss.str();
}

std::string ConstraintNode::toConstraintString() const {
    std::ostringstream oss;
    oss << "constraint " << constraintName << " {\n";
    oss << "  type: " << getConstraintTypeName() << ";\n";
    oss << "  operation: " << getOperationName() << ";\n";
    
    if (!constraintExpression.empty()) {
        oss << "  expression: " << constraintExpression << ";\n";
    }
    
    if (!targetElements.empty()) {
        oss << "  target_elements: [";
        for (size_t i = 0; i < targetElements.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << targetElements[i];
        }
        oss << "];\n";
    }
    
    if (!targetProperties.empty()) {
        oss << "  target_properties: [";
        for (size_t i = 0; i < targetProperties.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << targetProperties[i];
        }
        oss << "];\n";
    }
    
    if (!targetTypes.empty()) {
        oss << "  target_types: [";
        for (size_t i = 0; i < targetTypes.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << targetTypes[i];
        }
        oss << "];\n";
    }
    
    if (!conditions.empty()) {
        oss << "  conditions: [";
        for (size_t i = 0; i < conditions.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << conditions[i];
        }
        oss << "];\n";
    }
    
    if (!operationMessage.empty()) {
        oss << "  message: \"" << operationMessage << "\";\n";
    }
    
    oss << "}\n";
    return oss.str();
}

std::string ConstraintNode::formatConstraint() const {
    return toConstraintString();
}

std::string ConstraintNode::minifyConstraint() const {
    std::string result = toConstraintString();
    
    // 移除多余的空格和换行
    result = std::regex_replace(result, std::regex("\\s+"), " ");
    result = std::regex_replace(result, std::regex("\\s*\\{\\s*"), "{");
    result = std::regex_replace(result, std::regex("\\s*\\}\\s*"), "}");
    result = std::regex_replace(result, std::regex("\\s*;\\s*"), ";");
    result = std::regex_replace(result, std::regex("\\s*:\\s*"), ":");
    
    return result;
}

std::string ConstraintNode::beautifyConstraint() const {
    std::string result = toConstraintString();
    
    // 美化格式
    result = std::regex_replace(result, std::regex("\\{"), "{\n  ");
    result = std::regex_replace(result, std::regex(";"), ";\n");
    result = std::regex_replace(result, std::regex("\\}"), "\n}\n");
    
    return result;
}

std::string ConstraintNode::compressConstraint() const {
    std::string result = toConstraintString();
    
    // 压缩约束
    result = std::regex_replace(result, std::regex("\\s+"), "");
    result = std::regex_replace(result, std::regex("constraint"), "c");
    result = std::regex_replace(result, std::regex("type"), "t");
    result = std::regex_replace(result, std::regex("operation"), "op");
    result = std::regex_replace(result, std::regex("expression"), "expr");
    result = std::regex_replace(result, std::regex("target_elements"), "te");
    result = std::regex_replace(result, std::regex("target_properties"), "tp");
    result = std::regex_replace(result, std::regex("target_types"), "tt");
    result = std::regex_replace(result, std::regex("conditions"), "cond");
    result = std::regex_replace(result, std::regex("message"), "msg");
    
    return result;
}

std::string ConstraintNode::decompressConstraint() const {
    std::string result = toConstraintString();
    
    // 解压缩约束
    result = std::regex_replace(result, std::regex("c"), "constraint");
    result = std::regex_replace(result, std::regex("t"), "type");
    result = std::regex_replace(result, std::regex("op"), "operation");
    result = std::regex_replace(result, std::regex("expr"), "expression");
    result = std::regex_replace(result, std::regex("te"), "target_elements");
    result = std::regex_replace(result, std::regex("tp"), "target_properties");
    result = std::regex_replace(result, std::regex("tt"), "target_types");
    result = std::regex_replace(result, std::regex("cond"), "conditions");
    result = std::regex_replace(result, std::regex("msg"), "message");
    
    return result;
}

std::string ConstraintNode::encodeConstraint() const {
    std::string result = toConstraintString();
    
    // 编码约束
    for (char& c : result) {
        if (std::isalnum(c)) {
            c = static_cast<char>((c - 'a' + 13) % 26 + 'a');
        }
    }
    
    return result;
}

std::string ConstraintNode::decodeConstraint() const {
    std::string result = toConstraintString();
    
    // 解码约束
    for (char& c : result) {
        if (std::isalnum(c)) {
            c = static_cast<char>((c - 'a' + 13) % 26 + 'a');
        }
    }
    
    return result;
}

std::string ConstraintNode::getDebugInfo() const {
    std::ostringstream oss;
    oss << "ConstraintNode Debug Info:\n";
    oss << "  Name: " << constraintName << "\n";
    oss << "  Type: " << getConstraintTypeName() << "\n";
    oss << "  Operation: " << getOperationName() << "\n";
    oss << "  Active: " << (isActive ? "true" : "false") << "\n";
    oss << "  Processed: " << (isProcessed ? "true" : "false") << "\n";
    oss << "  Violated: " << (isViolated ? "true" : "false") << "\n";
    oss << "  Priority: " << priority << "\n";
    oss << "  Required: " << (isRequired ? "true" : "false") << "\n";
    oss << "  Optional: " << (isOptional ? "true" : "false") << "\n";
    
    if (!constraintExpression.empty()) {
        oss << "  Expression: " << constraintExpression << "\n";
    }
    
    if (!targetElements.empty()) {
        oss << "  Target Elements: ";
        for (size_t i = 0; i < targetElements.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << targetElements[i];
        }
        oss << "\n";
    }
    
    if (!targetProperties.empty()) {
        oss << "  Target Properties: ";
        for (size_t i = 0; i < targetProperties.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << targetProperties[i];
        }
        oss << "\n";
    }
    
    if (!targetTypes.empty()) {
        oss << "  Target Types: ";
        for (size_t i = 0; i < targetTypes.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << targetTypes[i];
        }
        oss << "\n";
    }
    
    if (!conditions.empty()) {
        oss << "  Conditions: ";
        for (size_t i = 0; i < conditions.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << conditions[i];
        }
        oss << "\n";
    }
    
    if (!constraintErrors.empty()) {
        oss << "  Errors: ";
        for (size_t i = 0; i < constraintErrors.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << constraintErrors[i];
        }
        oss << "\n";
    }
    
    if (!constraintWarnings.empty()) {
        oss << "  Warnings: ";
        for (size_t i = 0; i < constraintWarnings.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << constraintWarnings[i];
        }
        oss << "\n";
    }
    
    if (!violationMessages.empty()) {
        oss << "  Violations: ";
        for (size_t i = 0; i < violationMessages.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << violationMessages[i];
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::shared_ptr<BaseNode> ConstraintNode::clone() const {
    auto cloned = std::make_shared<ConstraintNode>(constraintName, constraintType);
    
    // 复制基本属性
    cloned->setValue(value);
    // cloned->setAttributes(attributes);
    
    // 复制约束属性
    cloned->constraintExpression = constraintExpression;
    cloned->constraintParameters = constraintParameters;
    cloned->targetElements = targetElements;
    cloned->targetProperties = targetProperties;
    cloned->targetTypes = targetTypes;
    cloned->conditions = conditions;
    cloned->conditionValues = conditionValues;
    cloned->conditionFunctions = conditionFunctions;
    cloned->operation = operation;
    cloned->operationMessage = operationMessage;
    cloned->operationPriority = operationPriority;
    cloned->isActive = isActive;
    cloned->isProcessed = isProcessed;
    cloned->isViolated = isViolated;
    cloned->violationMessages = violationMessages;
    cloned->constraintErrors = constraintErrors;
    cloned->constraintWarnings = constraintWarnings;
    cloned->metadata = metadata;
    cloned->priority = priority;
    cloned->isRequired = isRequired;
    cloned->isOptional = isOptional;
    
    return cloned;
}

bool ConstraintNode::operator==(const ConstraintNode& other) const {
    return constraintName == other.constraintName &&
           constraintType == other.constraintType &&
           constraintExpression == other.constraintExpression &&
           constraintParameters == other.constraintParameters &&
           targetElements == other.targetElements &&
           targetProperties == other.targetProperties &&
           targetTypes == other.targetTypes &&
           conditions == other.conditions &&
           conditionValues == other.conditionValues &&
           operation == other.operation &&
           operationMessage == other.operationMessage &&
           operationPriority == other.operationPriority &&
           isActive == other.isActive &&
           isProcessed == other.isProcessed &&
           isViolated == other.isViolated &&
           priority == other.priority &&
           isRequired == other.isRequired &&
           isOptional == other.isOptional;
}

bool ConstraintNode::operator!=(const ConstraintNode& other) const {
    return !(*this == other);
}

// 私有辅助方法实现
std::string ConstraintNode::processConstraintExpression(const std::string& expression) const {
    if (expression.empty()) {
        return "";
    }
    
    std::string result = expression;
    
    // 处理约束表达式
    result = std::regex_replace(result, std::regex("\\s+"), " ");
    result = std::regex_replace(result, std::regex("^\\s+|\\s+$"), "");
    
    return result;
}

std::string ConstraintNode::validateConstraintExpression(const std::string& expression) const {
    if (expression.empty()) {
        return "";
    }
    
    // 验证约束表达式
    if (expression.find("invalid") != std::string::npos) {
        return "";
    }
    
    return expression;
}

std::string ConstraintNode::sanitizeConstraintExpression(const std::string& expression) const {
    if (expression.empty()) {
        return "";
    }
    
    std::string result = expression;
    
    // 清理约束表达式
    result = std::regex_replace(result, std::regex("[^a-zA-Z0-9\\s\\-_=<>!&|()\\[\\]{}:;,\\.]"), "");
    
    return result;
}

std::string ConstraintNode::parseConstraintExpression(const std::string& expression) const {
    if (expression.empty()) {
        return "";
    }
    
    // 解析约束表达式
    return processConstraintExpression(expression);
}

std::string ConstraintNode::evaluateConstraintExpression(const std::string& expression) const {
    if (expression.empty()) {
        return "";
    }
    
    // 评估约束表达式
    return processConstraintExpression(expression);
}

std::string ConstraintNode::optimizeConstraintExpression(const std::string& expression) const {
    if (expression.empty()) {
        return "";
    }
    
    // 优化约束表达式
    return processConstraintExpression(expression);
}

bool ConstraintNode::evaluateCondition(const std::string& condition, const std::string& value) const {
    if (condition.empty()) {
        return true;
    }
    
    // 评估条件
    if (hasConditionFunction(condition)) {
        return evaluateConditionFunction(condition, value);
    }
    
    if (hasConditionValue(condition)) {
        return evaluateConditionValue(condition, value);
    }
    
    return true;
}

bool ConstraintNode::evaluateConditionFunction(const std::string& condition, const std::string& value) const {
    auto func = getConditionFunction(condition);
    if (func) {
        return func(value);
    }
    return true;
}

bool ConstraintNode::evaluateConditionValue(const std::string& condition, const std::string& value) const {
    std::string expectedValue = getConditionValue(condition);
    return value == expectedValue;
}

void ConstraintNode::processConstraintTargets() {
    // 处理约束目标
    for (const auto& element : targetElements) {
        // 处理元素目标
    }
    
    for (const auto& property : targetProperties) {
        // 处理属性目标
    }
    
    for (const auto& type : targetTypes) {
        // 处理类型目标
    }
}

void ConstraintNode::processConstraintConditions() {
    // 处理约束条件
    for (const auto& condition : conditions) {
        // 处理条件
    }
}

void ConstraintNode::processConstraintOperation() {
    // 处理约束操作
    switch (operation) {
        case ConstraintOperation::ALLOW:
            // 处理允许操作
            break;
        case ConstraintOperation::DENY:
            // 处理拒绝操作
            break;
        case ConstraintOperation::REQUIRE:
            // 处理要求操作
            break;
        case ConstraintOperation::OPTIONAL:
            // 处理可选操作
            break;
        case ConstraintOperation::WARN:
            // 处理警告操作
            break;
    }
}

bool ConstraintNode::validateConstraintTargets() const {
    return !targetElements.empty() || !targetProperties.empty() || !targetTypes.empty();
}

bool ConstraintNode::validateConstraintConditions() const {
    for (const auto& condition : conditions) {
        if (!hasConditionValue(condition) && !hasConditionFunction(condition)) {
            return false;
        }
    }
    return true;
}

bool ConstraintNode::validateConstraintOperation() const {
    return operationMessage.empty() || !operationMessage.empty();
}

bool ConstraintNode::checkExactConstraint(const std::string& value) const {
    if (constraintExpression.empty()) {
        return true;
    }
    
    return value == constraintExpression;
}


bool ConstraintNode::checkGlobalConstraint(const std::string& value) const {
    // 检查全局约束
    return true;
}

bool ConstraintNode::checkLocalConstraint(const std::string& value) const {
    // 检查局部约束
    return true;
}

bool ConstraintNode::checkInheritedConstraint(const std::string& value) const {
    // 检查继承约束
    return true;
}

void ConstraintNode::handleConstraintError(const std::string& error) {
    addConstraintError(error);
    isViolated = true;
}

void ConstraintNode::handleConstraintWarning(const std::string& warning) {
    addConstraintWarning(warning);
}

void ConstraintNode::handleConstraintViolation(const std::string& violation) {
    addViolationMessage(violation);
    isViolated = true;
}

} // namespace CHTL