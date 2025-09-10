#include "chtl/constraint_system.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace chtl {
namespace constraint_system {

// ConstraintParser 实现
ConstraintParser::ConstraintParser(const std::string& input) : input_(input), position_(0) {}

std::vector<std::shared_ptr<ConstraintRule>> ConstraintParser::parse() {
    std::vector<std::shared_ptr<ConstraintRule>> rules;
    
    while (position_ < input_.length()) {
        skipWhitespace();
        if (position_ >= input_.length()) break;
        
        if (input_.substr(position_, 6) == "except") {
            position_ += 6;
            skipWhitespace();
            
            auto rule = parseConstraintRule();
            if (rule) {
                rules.push_back(rule);
            }
        } else {
            advance();
        }
    }
    
    return rules;
}

void ConstraintParser::skipWhitespace() {
    while (position_ < input_.length() && std::isspace(input_[position_])) {
        position_++;
    }
}

char ConstraintParser::currentChar() const {
    if (position_ >= input_.length()) return '\0';
    return input_[position_];
}

char ConstraintParser::peekChar() const {
    if (position_ + 1 >= input_.length()) return '\0';
    return input_[position_ + 1];
}

void ConstraintParser::advance() {
    if (position_ < input_.length()) {
        position_++;
    }
}

std::shared_ptr<ConstraintRule> ConstraintParser::parseConstraintRule() {
    auto rule = std::make_shared<ConstraintRule>();
    
    skipWhitespace();
    
    // 解析约束名称
    rule->name = parseIdentifier();
    skipWhitespace();
    
    // 解析约束类型
    if (input_.substr(position_, 7) == "precise") {
        position_ += 7;
        rule->type = ConstraintType::PRECISE;
    } else if (input_.substr(position_, 4) == "type") {
        position_ += 4;
        rule->type = ConstraintType::TYPE;
    } else if (input_.substr(position_, 6) == "global") {
        position_ += 6;
        rule->type = ConstraintType::GLOBAL;
    }
    
    skipWhitespace();
    
    // 解析约束目标
    if (input_.substr(position_, 4) == "html") {
        position_ += 4;
        rule->target = ConstraintTarget::HTML_ELEMENT;
    } else if (input_.substr(position_, 6) == "custom") {
        position_ += 6;
        rule->target = ConstraintTarget::CUSTOM_OBJECT;
    } else if (input_.substr(position_, 7) == "template") {
        position_ += 7;
        rule->target = ConstraintTarget::TEMPLATE_OBJECT;
    } else if (input_.substr(position_, 4) == "type") {
        position_ += 4;
        rule->target = ConstraintTarget::TYPE;
    }
    
    skipWhitespace();
    
    // 解析约束条件
    if (currentChar() == '{') {
        advance();
        rule->conditions = parseConditions();
        skipWhitespace();
        if (currentChar() == '}') advance();
    }
    
    // 解析作用域
    if (input_.substr(position_, 5) == "scope") {
        position_ += 5;
        skipWhitespace();
        if (currentChar() == ':') {
            advance();
            skipWhitespace();
            rule->scope = parseString();
        }
    }
    
    return rule;
}

std::vector<ConstraintCondition> ConstraintParser::parseConditions() {
    std::vector<ConstraintCondition> conditions;
    
    while (position_ < input_.length() && currentChar() != '}') {
        skipWhitespace();
        if (currentChar() == '}') break;
        
        conditions.push_back(parseCondition());
        skipWhitespace();
        
        if (currentChar() == ',') {
            advance();
            skipWhitespace();
        }
    }
    
    return conditions;
}

ConstraintCondition ConstraintParser::parseCondition() {
    std::string name = parseIdentifier();
    skipWhitespace();
    
    std::string operator_ = parseOperator();
    skipWhitespace();
    
    std::string value = parseString();
    
    return ConstraintCondition(name, value, operator_);
}

std::string ConstraintParser::parseString() {
    std::string result;
    
    if (currentChar() == '"') {
        advance();
        while (position_ < input_.length() && currentChar() != '"') {
            result += currentChar();
            advance();
        }
        if (currentChar() == '"') advance();
    } else if (currentChar() == '\'') {
        advance();
        while (position_ < input_.length() && currentChar() != '\'') {
            result += currentChar();
            advance();
        }
        if (currentChar() == '\'') advance();
    } else {
        // 解析无引号字符串
        while (position_ < input_.length() && 
               !std::isspace(currentChar()) && 
               currentChar() != ',' && 
               currentChar() != '}' && 
               currentChar() != ';') {
            result += currentChar();
            advance();
        }
    }
    
    return result;
}

std::string ConstraintParser::parseIdentifier() {
    std::string result;
    
    while (position_ < input_.length() && 
           (std::isalnum(currentChar()) || currentChar() == '_' || currentChar() == '-')) {
        result += currentChar();
        advance();
    }
    
    return result;
}

std::string ConstraintParser::parseOperator() {
    std::string result;
    
    if (currentChar() == '=' && peekChar() == '=') {
        result = "==";
        advance();
        advance();
    } else if (currentChar() == '!' && peekChar() == '=') {
        result = "!=";
        advance();
        advance();
    } else if (currentChar() == '<' && peekChar() == '=') {
        result = "<=";
        advance();
        advance();
    } else if (currentChar() == '>' && peekChar() == '=') {
        result = ">=";
        advance();
        advance();
    } else if (currentChar() == '<') {
        result = "<";
        advance();
    } else if (currentChar() == '>') {
        result = ">";
        advance();
    } else if (input_.substr(position_, 2) == "in") {
        result = "in";
        position_ += 2;
    } else if (input_.substr(position_, 6) == "not_in") {
        result = "not_in";
        position_ += 6;
    } else {
        result = currentChar();
        advance();
    }
    
    return result;
}

bool ConstraintParser::isKeyword(const std::string& word) const {
    static const std::vector<std::string> keywords = {
        "except", "precise", "type", "global", "html", "custom", "template", "scope"
    };
    
    return std::find(keywords.begin(), keywords.end(), word) != keywords.end();
}

bool ConstraintParser::isConstraintKeyword(const std::string& word) const {
    static const std::vector<std::string> constraintKeywords = {
        "except", "precise", "type", "global", "html", "custom", "template", "scope"
    };
    
    return std::find(constraintKeywords.begin(), constraintKeywords.end(), word) != constraintKeywords.end();
}

// ConstraintCompiler 实现
ConstraintCompiler::ConstraintCompiler() {}

std::string ConstraintCompiler::compile(const std::vector<std::shared_ptr<ConstraintRule>>& rules) {
    std::string result = "// Generated Constraint Rules\n\n";
    
    for (const auto& rule : rules) {
        result += compileConstraintRule(*rule) + "\n";
    }
    
    return result;
}

std::string ConstraintCompiler::compileConstraintRule(const ConstraintRule& rule) {
    std::string result = "// Constraint Rule: " + rule.name + "\n";
    result += "// Type: " + std::to_string(static_cast<int>(rule.type)) + "\n";
    result += "// Target: " + std::to_string(static_cast<int>(rule.target)) + "\n";
    
    if (!rule.scope.empty()) {
        result += "// Scope: " + rule.scope + "\n";
    }
    
    result += "function validate_" + rule.name + "() {\n";
    result += "    // Validation logic\n";
    
    for (const auto& condition : rule.conditions) {
        result += "    " + compileCondition(condition) + "\n";
    }
    
    result += "}\n\n";
    return result;
}

std::string ConstraintCompiler::compileCondition(const ConstraintCondition& condition) {
    return "if (" + condition.name + " " + condition.operator_ + " " + condition.value + ") { /* valid */ }";
}

void ConstraintCompiler::addRule(std::shared_ptr<ConstraintRule> rule) {
    rules_[rule->name] = rule;
}

std::shared_ptr<ConstraintRule> ConstraintCompiler::getRule(const std::string& name) {
    auto it = rules_.find(name);
    return (it != rules_.end()) ? it->second : nullptr;
}

bool ConstraintCompiler::validate(const std::string& target, const std::map<std::string, std::string>& attributes) {
    for (const auto& rule : rules_) {
        if (!rule.second->isActive) continue;
        
        switch (rule.second->type) {
            case ConstraintType::PRECISE:
                if (!validatePreciseConstraint(*rule.second, target, attributes)) return false;
                break;
            case ConstraintType::TYPE:
                if (!validateTypeConstraint(*rule.second, target, attributes)) return false;
                break;
            case ConstraintType::GLOBAL:
                if (!validateGlobalConstraint(*rule.second, target, attributes)) return false;
                break;
        }
    }
    
    return true;
}

bool ConstraintCompiler::validatePreciseConstraint(const ConstraintRule& rule, const std::string& target, const std::map<std::string, std::string>& attributes) {
    // 精确约束验证
    for (const auto& condition : rule.conditions) {
        if (!evaluateCondition(condition, attributes)) {
            return false;
        }
    }
    return true;
}

bool ConstraintCompiler::validateTypeConstraint(const ConstraintRule& rule, const std::string& target, const std::map<std::string, std::string>& attributes) {
    // 类型约束验证
    for (const auto& condition : rule.conditions) {
        if (!evaluateCondition(condition, attributes)) {
            return false;
        }
    }
    return true;
}

bool ConstraintCompiler::validateGlobalConstraint(const ConstraintRule& rule, const std::string& target, const std::map<std::string, std::string>& attributes) {
    // 全局约束验证
    for (const auto& condition : rule.conditions) {
        if (!evaluateCondition(condition, attributes)) {
            return false;
        }
    }
    return true;
}

std::string ConstraintCompiler::generateValidationCode(const ConstraintRule& rule) {
    return "// Validation code for " + rule.name;
}

std::string ConstraintCompiler::generateConditionCode(const ConstraintCondition& condition) {
    return condition.name + " " + condition.operator_ + " " + condition.value;
}

bool ConstraintCompiler::evaluateCondition(const ConstraintCondition& condition, const std::map<std::string, std::string>& attributes) {
    auto it = attributes.find(condition.name);
    if (it == attributes.end()) return false;
    
    std::string actualValue = it->second;
    std::string expectedValue = condition.value;
    
    if (condition.operator_ == "==") {
        return actualValue == expectedValue;
    } else if (condition.operator_ == "!=") {
        return actualValue != expectedValue;
    } else if (condition.operator_ == "<") {
        return actualValue < expectedValue;
    } else if (condition.operator_ == ">") {
        return actualValue > expectedValue;
    } else if (condition.operator_ == "<=") {
        return actualValue <= expectedValue;
    } else if (condition.operator_ == ">=") {
        return actualValue >= expectedValue;
    } else if (condition.operator_ == "in") {
        return expectedValue.find(actualValue) != std::string::npos;
    } else if (condition.operator_ == "not_in") {
        return expectedValue.find(actualValue) == std::string::npos;
    }
    
    return false;
}

// ConstraintManager 实现
ConstraintManager::ConstraintManager() {}

void ConstraintManager::addRule(std::shared_ptr<ConstraintRule> rule) {
    compiler_.addRule(rule);
}

std::shared_ptr<ConstraintRule> ConstraintManager::getRule(const std::string& name) {
    return compiler_.getRule(name);
}

std::string ConstraintManager::generateCode(const std::vector<std::shared_ptr<ConstraintRule>>& rules) {
    return compiler_.compile(rules);
}

std::string ConstraintManager::generateValidationCode() {
    std::string result = "// Constraint Validation Code\n\n";
    
    for (const auto& rule : compiler_.rules_) {
        result += "function validate_" + rule.first + "() {\n";
        result += "    // Validation logic for " + rule.first + "\n";
        result += "}\n\n";
    }
    
    return result;
}

bool ConstraintManager::validateTarget(const std::string& target, const std::map<std::string, std::string>& attributes) {
    return compiler_.validate(target, attributes);
}

bool ConstraintManager::validateHTMLElement(const std::string& element, const std::map<std::string, std::string>& attributes) {
    return compiler_.validate(element, attributes);
}

bool ConstraintManager::validateCustomObject(const std::string& object, const std::map<std::string, std::string>& attributes) {
    return compiler_.validate(object, attributes);
}

bool ConstraintManager::validateTemplateObject(const std::string& object, const std::map<std::string, std::string>& attributes) {
    return compiler_.validate(object, attributes);
}

bool ConstraintManager::validateType(const std::string& type, const std::map<std::string, std::string>& attributes) {
    return compiler_.validate(type, attributes);
}

void ConstraintManager::clear() {
    compiler_.rules_.clear();
}

size_t ConstraintManager::getRuleCount() const {
    return compiler_.rules_.size();
}

std::string ConstraintManager::processConstraintScope(const std::string& scope) {
    return scope;
}

std::string ConstraintManager::resolveConstraintDependencies(const std::string& content) {
    return content;
}

std::string ConstraintManager::validateConstraintParameters(const std::string& ruleName, const std::map<std::string, std::string>& parameters) {
    return "";
}

// ConstraintValidator 实现
ConstraintValidator::ConstraintValidator() {}

void ConstraintValidator::setRules(const std::vector<std::shared_ptr<ConstraintRule>>& rules) {
    rules_ = rules;
}

bool ConstraintValidator::validate(const std::string& target, const std::map<std::string, std::string>& attributes) {
    clearErrors();
    clearWarnings();
    
    bool isValid = true;
    
    for (const auto& rule : rules_) {
        if (!rule->isActive) continue;
        
        if (!checkConstraint(*rule, target, attributes)) {
            addError("Constraint violation: " + rule->name);
            isValid = false;
        }
    }
    
    return isValid;
}

std::vector<std::string> ConstraintValidator::getValidationErrors() {
    return errors_;
}

std::vector<std::string> ConstraintValidator::getValidationWarnings() {
    return warnings_;
}

void ConstraintValidator::clearErrors() {
    errors_.clear();
}

void ConstraintValidator::clearWarnings() {
    warnings_.clear();
}

void ConstraintValidator::addError(const std::string& error) {
    errors_.push_back(error);
}

void ConstraintValidator::addWarning(const std::string& warning) {
    warnings_.push_back(warning);
}

bool ConstraintValidator::checkConstraint(const ConstraintRule& rule, const std::string& target, const std::map<std::string, std::string>& attributes) {
    // 检查约束条件
    for (const auto& condition : rule.conditions) {
        auto it = attributes.find(condition.name);
        if (it == attributes.end()) {
            addError("Missing attribute: " + condition.name);
            return false;
        }
        
        std::string actualValue = it->second;
        std::string expectedValue = condition.value;
        
        bool conditionMet = false;
        
        if (condition.operator_ == "==") {
            conditionMet = (actualValue == expectedValue);
        } else if (condition.operator_ == "!=") {
            conditionMet = (actualValue != expectedValue);
        } else if (condition.operator_ == "<") {
            conditionMet = (actualValue < expectedValue);
        } else if (condition.operator_ == ">") {
            conditionMet = (actualValue > expectedValue);
        } else if (condition.operator_ == "<=") {
            conditionMet = (actualValue <= expectedValue);
        } else if (condition.operator_ == ">=") {
            conditionMet = (actualValue >= expectedValue);
        } else if (condition.operator_ == "in") {
            conditionMet = (expectedValue.find(actualValue) != std::string::npos);
        } else if (condition.operator_ == "not_in") {
            conditionMet = (expectedValue.find(actualValue) == std::string::npos);
        }
        
        if (!conditionMet) {
            addError("Condition not met: " + condition.name + " " + condition.operator_ + " " + condition.value);
            return false;
        }
    }
    
    return true;
}

} // namespace constraint_system
} // namespace chtl