#include "ConstraintNode.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

ConstraintNode::ConstraintNode(ConstraintType type, ConstraintTarget target, const std::string& expression) 
    : BaseNode(NodeType::Constraint, "constraint"), constraintType(type), target(target), 
      constraintExpression(expression), isEnabled(true), priority(0) {
}

void ConstraintNode::addExcept(const std::string& exception) {
    exceptList.push_back(exception);
}

void ConstraintNode::removeExcept(const std::string& exception) {
    auto it = std::find(exceptList.begin(), exceptList.end(), exception);
    if (it != exceptList.end()) {
        exceptList.erase(it);
    }
}

void ConstraintNode::addRule(const std::string& rule, const std::string& value) {
    constraintRules[rule] = value;
}

void ConstraintNode::removeRule(const std::string& rule) {
    constraintRules.erase(rule);
}

std::string ConstraintNode::getRule(const std::string& rule) const {
    auto it = constraintRules.find(rule);
    if (it != constraintRules.end()) {
        return it->second;
    }
    return "";
}

bool ConstraintNode::hasRule(const std::string& rule) const {
    return constraintRules.find(rule) != constraintRules.end();
}

bool ConstraintNode::validate(const std::string& target) const {
    if (!isEnabled) {
        return true;
    }
    
    // 检查是否在例外列表中
    if (isInExceptList(target)) {
        return true;
    }
    
    // 检查是否匹配约束表达式
    return matchesConstraintExpression(target);
}

bool ConstraintNode::validateElement(const std::string& elementName) const {
    if (target != ConstraintTarget::ELEMENT && target != ConstraintTarget::ALL) {
        return true;
    }
    
    return validate(elementName);
}

bool ConstraintNode::validateAttribute(const std::string& attributeName) const {
    if (target != ConstraintTarget::ATTRIBUTE && target != ConstraintTarget::ALL) {
        return true;
    }
    
    return validate(attributeName);
}

bool ConstraintNode::validateStyle(const std::string& styleName) const {
    if (target != ConstraintTarget::STYLE && target != ConstraintTarget::ALL) {
        return true;
    }
    
    return validate(styleName);
}

bool ConstraintNode::validateScript(const std::string& scriptName) const {
    if (target != ConstraintTarget::SCRIPT && target != ConstraintTarget::ALL) {
        return true;
    }
    
    return validate(scriptName);
}

bool ConstraintNode::appliesTo(const std::string& target) const {
    if (!isEnabled) {
        return false;
    }
    
    return matchesConstraintExpression(target);
}

bool ConstraintNode::appliesToElement(const std::string& elementName) const {
    if (target != ConstraintTarget::ELEMENT && target != ConstraintTarget::ALL) {
        return false;
    }
    
    return appliesTo(elementName);
}

bool ConstraintNode::appliesToAttribute(const std::string& attributeName) const {
    if (target != ConstraintTarget::ATTRIBUTE && target != ConstraintTarget::ALL) {
        return false;
    }
    
    return appliesTo(attributeName);
}

bool ConstraintNode::appliesToStyle(const std::string& styleName) const {
    if (target != ConstraintTarget::STYLE && target != ConstraintTarget::ALL) {
        return false;
    }
    
    return appliesTo(styleName);
}

bool ConstraintNode::appliesToScript(const std::string& scriptName) const {
    if (target != ConstraintTarget::SCRIPT && target != ConstraintTarget::ALL) {
        return false;
    }
    
    return appliesTo(scriptName);
}

std::vector<std::string> ConstraintNode::parseConstraintExpression() const {
    return tokenizeExpression(constraintExpression);
}

bool ConstraintNode::evaluateConstraint(const std::string& target) const {
    return matchesConstraintExpression(target);
}

std::shared_ptr<BaseNode> ConstraintNode::clone() const {
    auto cloned = std::make_shared<ConstraintNode>(constraintType, target, constraintExpression);
    cloned->exceptList = exceptList;
    cloned->constraintRules = constraintRules;
    cloned->isEnabled = isEnabled;
    cloned->scope = scope;
    cloned->priority = priority;
    cloned->line = line;
    cloned->column = column;
    cloned->position = position;
    
    return cloned;
}

std::string ConstraintNode::toString() const {
    std::ostringstream oss;
    oss << "ConstraintNode(" << constraintTypeToString(constraintType) 
        << ", " << constraintTargetToString(target) << ", \"" << constraintExpression << "\")";
    return oss.str();
}

std::string ConstraintNode::toHTML() const {
    // 约束节点不直接生成HTML
    return "";
}

std::string ConstraintNode::toCSS() const {
    // 约束节点不直接生成CSS
    return "";
}

std::string ConstraintNode::toJS() const {
    // 约束节点不直接生成JS
    return "";
}

bool ConstraintNode::isValid() const {
    return BaseNode::isValid() && !constraintExpression.empty();
}

std::string ConstraintNode::debugString() const {
    std::ostringstream oss;
    oss << "ConstraintNode: " << constraintExpression 
        << " (Type: " << constraintTypeToString(constraintType) 
        << ", Target: " << constraintTargetToString(target) 
        << ", Enabled: " << (isEnabled ? "true" : "false") 
        << ", Line: " << line << ", Column: " << column << ")";
    return oss.str();
}

ConstraintType ConstraintNode::parseConstraintType(const std::string& type) {
    if (type == "precise") {
        return ConstraintType::PRECISE;
    } else if (type == "type") {
        return ConstraintType::TYPE;
    } else if (type == "global") {
        return ConstraintType::GLOBAL;
    }
    return ConstraintType::PRECISE;
}

ConstraintTarget ConstraintNode::parseConstraintTarget(const std::string& target) {
    if (target == "element") {
        return ConstraintTarget::ELEMENT;
    } else if (target == "attribute") {
        return ConstraintTarget::ATTRIBUTE;
    } else if (target == "style") {
        return ConstraintTarget::STYLE;
    } else if (target == "script") {
        return ConstraintTarget::SCRIPT;
    } else if (target == "template") {
        return ConstraintTarget::TEMPLATE;
    } else if (target == "custom") {
        return ConstraintTarget::CUSTOM;
    } else if (target == "namespace") {
        return ConstraintTarget::NAMESPACE;
    } else if (target == "all") {
        return ConstraintTarget::ALL;
    }
    return ConstraintTarget::ALL;
}

std::string ConstraintNode::constraintTypeToString(ConstraintType type) {
    switch (type) {
        case ConstraintType::PRECISE: return "precise";
        case ConstraintType::TYPE: return "type";
        case ConstraintType::GLOBAL: return "global";
        default: return "unknown";
    }
}

std::string ConstraintNode::constraintTargetToString(ConstraintTarget target) {
    switch (target) {
        case ConstraintTarget::ELEMENT: return "element";
        case ConstraintTarget::ATTRIBUTE: return "attribute";
        case ConstraintTarget::STYLE: return "style";
        case ConstraintTarget::SCRIPT: return "script";
        case ConstraintTarget::TEMPLATE: return "template";
        case ConstraintTarget::CUSTOM: return "custom";
        case ConstraintTarget::NAMESPACE: return "namespace";
        case ConstraintTarget::ALL: return "all";
        default: return "unknown";
    }
}

bool ConstraintNode::isInExceptList(const std::string& target) const {
    return std::find(exceptList.begin(), exceptList.end(), target) != exceptList.end();
}

bool ConstraintNode::matchesConstraintExpression(const std::string& target) const {
    if (constraintExpression.empty()) {
        return true;
    }
    
    // 简单的通配符匹配
    if (constraintExpression == "*") {
        return true;
    }
    
    // 精确匹配
    if (constraintExpression == target) {
        return true;
    }
    
    // 前缀匹配
    if (constraintExpression.back() == '*') {
        std::string prefix = constraintExpression.substr(0, constraintExpression.length() - 1);
        return target.find(prefix) == 0;
    }
    
    // 后缀匹配
    if (constraintExpression.front() == '*') {
        std::string suffix = constraintExpression.substr(1);
        return target.length() >= suffix.length() && 
               target.substr(target.length() - suffix.length()) == suffix;
    }
    
    // 包含匹配
    if (constraintExpression.front() == '*' && constraintExpression.back() == '*') {
        std::string pattern = constraintExpression.substr(1, constraintExpression.length() - 2);
        return target.find(pattern) != std::string::npos;
    }
    
    // 正则表达式匹配
    try {
        std::regex pattern(constraintExpression);
        return std::regex_match(target, pattern);
    } catch (const std::regex_error&) {
        // 如果正则表达式无效，回退到精确匹配
        return constraintExpression == target;
    }
}

std::vector<std::string> ConstraintNode::tokenizeExpression(const std::string& expression) const {
    std::vector<std::string> tokens;
    std::string current;
    
    for (char c : expression) {
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        tokens.push_back(current);
    }
    
    return tokens;
}

bool ConstraintNode::evaluateToken(const std::string& token, const std::string& target) const {
    if (token == "*") {
        return true;
    }
    
    if (token == target) {
        return true;
    }
    
    if (token.front() == '*' && token.back() == '*') {
        std::string pattern = token.substr(1, token.length() - 2);
        return target.find(pattern) != std::string::npos;
    }
    
    if (token.back() == '*') {
        std::string prefix = token.substr(0, token.length() - 1);
        return target.find(prefix) == 0;
    }
    
    if (token.front() == '*') {
        std::string suffix = token.substr(1);
        return target.length() >= suffix.length() && 
               target.substr(target.length() - suffix.length()) == suffix;
    }
    
    return false;
}

} // namespace CHTL