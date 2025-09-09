#include "ConstraintManager.hpp"
#include <sstream>
#include <regex>
#include <algorithm>

namespace CHTL {

ConstraintManager& ConstraintManager::getInstance() {
    static ConstraintManager instance;
    return instance;
}

void ConstraintManager::addConstraint(const std::string& name, ConstraintType type, ConstraintScope scope,
                                     const std::string& target, 
                                     std::function<bool(const std::string&, const std::map<std::string, std::string>&)> validator,
                                     const std::string& errorMessage) {
    if (!validateConstraintName(name) || !validateConstraintTarget(target)) {
        return;
    }
    
    auto constraintDef = std::make_shared<ConstraintDefinition>();
    constraintDef->name = name;
    constraintDef->type = type;
    constraintDef->scope = scope;
    constraintDef->target = target;
    constraintDef->validator = validator;
    constraintDef->errorMessage = errorMessage.empty() ? "Constraint validation failed" : errorMessage;
    constraintDef->isActive = true;
    
    constraints_[name] = constraintDef;
}

void ConstraintManager::addPreciseConstraint(const std::string& name, ConstraintScope scope,
                                            const std::string& target, const std::string& expectedValue,
                                            const std::string& errorMessage) {
    auto validator = [expectedValue](const std::string& value, const std::map<std::string, std::string>&) {
        return value == expectedValue;
    };
    
    addConstraint(name, ConstraintType::PRECISE, scope, target, validator, errorMessage);
}

void ConstraintManager::addTypeConstraint(const std::string& name, ConstraintScope scope,
                                         const std::string& target, const std::string& expectedType,
                                         const std::string& errorMessage) {
    auto validator = [expectedType](const std::string& value, const std::map<std::string, std::string>&) {
        // 根据类型进行验证
        if (expectedType == "string") {
            return !value.empty();
        } else if (expectedType == "number") {
            return std::regex_match(value, std::regex(R"(\d+(\.\d+)?)"));
        } else if (expectedType == "boolean") {
            return value == "true" || value == "false";
        } else if (expectedType == "color") {
            return std::regex_match(value, std::regex(R"(rgb\([^)]+\)|#[0-9a-fA-F]{3,6}|[a-zA-Z]+)"));
        } else if (expectedType == "length") {
            return std::regex_match(value, std::regex(R"(\d+(\.\d+)?(px|em|rem|%|vh|vw|pt|pc|in|cm|mm))"));
        } else if (expectedType == "url") {
            return std::regex_match(value, std::regex(R"(https?://[^\s]+|/[^\s]*|\./[^\s]*|\.\./[^\s]*)"));
        }
        return true;
    };
    
    addConstraint(name, ConstraintType::TYPE, scope, target, validator, errorMessage);
}

void ConstraintManager::addGlobalConstraint(const std::string& name, ConstraintScope scope,
                                           const std::string& target, 
                                           std::function<bool(const std::string&, const std::map<std::string, std::string>&)> validator,
                                           const std::string& errorMessage) {
    addConstraint(name, ConstraintType::GLOBAL, scope, target, validator, errorMessage);
}

std::shared_ptr<ConstraintManager::ConstraintDefinition> ConstraintManager::getConstraint(const std::string& name) const {
    auto it = constraints_.find(name);
    return (it != constraints_.end()) ? it->second : nullptr;
}

bool ConstraintManager::hasConstraint(const std::string& name) const {
    return constraints_.find(name) != constraints_.end();
}

bool ConstraintManager::validateConstraint(const std::string& name, const std::string& value, 
                                          const std::map<std::string, std::string>& context) const {
    auto constraintDef = getConstraint(name);
    if (!constraintDef || !constraintDef->isActive) {
        return true;
    }
    
    switch (constraintDef->type) {
        case ConstraintType::PRECISE:
            return validatePreciseConstraint(*constraintDef, value);
        case ConstraintType::TYPE:
            return validateTypeConstraint(*constraintDef, value);
        case ConstraintType::GLOBAL:
            return validateGlobalConstraint(*constraintDef, value, context);
        default:
            return true;
    }
}

std::vector<std::string> ConstraintManager::validateAllConstraints(const std::string& value, 
                                                                  ConstraintScope scope,
                                                                  const std::map<std::string, std::string>& context) const {
    std::vector<std::string> errors;
    
    for (const auto& [name, constraintDef] : constraints_) {
        if (constraintDef->isActive && constraintDef->scope == scope) {
            if (!validateConstraint(name, value, context)) {
                errors.push_back(constraintDef->errorMessage);
            }
        }
    }
    
    return errors;
}

std::string ConstraintManager::getConstraintErrorMessage(const std::string& name) const {
    auto constraintDef = getConstraint(name);
    return constraintDef ? constraintDef->errorMessage : "";
}

void ConstraintManager::activateConstraint(const std::string& name) {
    auto constraintDef = getConstraint(name);
    if (constraintDef) {
        constraintDef->isActive = true;
    }
}

void ConstraintManager::deactivateConstraint(const std::string& name) {
    auto constraintDef = getConstraint(name);
    if (constraintDef) {
        constraintDef->isActive = false;
    }
}

bool ConstraintManager::isConstraintActive(const std::string& name) const {
    auto constraintDef = getConstraint(name);
    return constraintDef && constraintDef->isActive;
}

std::vector<std::string> ConstraintManager::getAllConstraintNames() const {
    std::vector<std::string> names;
    for (const auto& [name, constraintDef] : constraints_) {
        names.push_back(name);
    }
    return names;
}

std::vector<std::string> ConstraintManager::getConstraintNamesByType(ConstraintType type) const {
    std::vector<std::string> names;
    for (const auto& [name, constraintDef] : constraints_) {
        if (constraintDef->type == type) {
            names.push_back(name);
        }
    }
    return names;
}

std::vector<std::string> ConstraintManager::getConstraintNamesByScope(ConstraintScope scope) const {
    std::vector<std::string> names;
    for (const auto& [name, constraintDef] : constraints_) {
        if (constraintDef->scope == scope) {
            names.push_back(name);
        }
    }
    return names;
}

void ConstraintManager::clearConstraint(const std::string& name) {
    auto constraintDef = getConstraint(name);
    if (constraintDef) {
        constraintDef->parameters.clear();
        constraintDef->isActive = false;
    }
}

void ConstraintManager::clear() {
    constraints_.clear();
    constraintGroups_.clear();
}

void ConstraintManager::removeConstraint(const std::string& name) {
    constraints_.erase(name);
    
    // 从约束组中移除
    for (auto& [groupName, constraintNames] : constraintGroups_) {
        auto it = std::find(constraintNames.begin(), constraintNames.end(), name);
        if (it != constraintNames.end()) {
            constraintNames.erase(it);
        }
    }
}

void ConstraintManager::setConstraintParameter(const std::string& name, const std::string& paramName, 
                                              const std::string& paramValue) {
    auto constraintDef = getConstraint(name);
    if (constraintDef) {
        constraintDef->parameters[paramName] = paramValue;
    }
}

std::string ConstraintManager::getConstraintParameter(const std::string& name, const std::string& paramName) const {
    auto constraintDef = getConstraint(name);
    if (constraintDef) {
        auto it = constraintDef->parameters.find(paramName);
        if (it != constraintDef->parameters.end()) {
            return it->second;
        }
    }
    return "";
}

std::map<std::string, std::string> ConstraintManager::getConstraintParameters(const std::string& name) const {
    auto constraintDef = getConstraint(name);
    return constraintDef ? constraintDef->parameters : std::map<std::string, std::string>();
}

void ConstraintManager::addConstraintGroup(const std::string& groupName, const std::vector<std::string>& constraintNames) {
    constraintGroups_[groupName] = constraintNames;
}

std::vector<std::string> ConstraintManager::getConstraintGroup(const std::string& groupName) const {
    auto it = constraintGroups_.find(groupName);
    return (it != constraintGroups_.end()) ? it->second : std::vector<std::string>();
}

std::vector<std::string> ConstraintManager::validateConstraintGroup(const std::string& groupName, 
                                                                   const std::string& value,
                                                                   const std::map<std::string, std::string>& context) const {
    std::vector<std::string> errors;
    auto constraintNames = getConstraintGroup(groupName);
    
    for (const auto& constraintName : constraintNames) {
        if (!validateConstraint(constraintName, value, context)) {
            errors.push_back(getConstraintErrorMessage(constraintName));
        }
    }
    
    return errors;
}

std::vector<std::string> ConstraintManager::getAllConstraintGroupNames() const {
    std::vector<std::string> names;
    for (const auto& [name, constraintNames] : constraintGroups_) {
        names.push_back(name);
    }
    return names;
}

void ConstraintManager::clearConstraintGroup(const std::string& groupName) {
    constraintGroups_.erase(groupName);
}

bool ConstraintManager::validatePreciseConstraint(const ConstraintDefinition& constraint, 
                                                 const std::string& value) const {
    return constraint.validator(value, constraint.parameters);
}

bool ConstraintManager::validateTypeConstraint(const ConstraintDefinition& constraint, 
                                              const std::string& value) const {
    return constraint.validator(value, constraint.parameters);
}

bool ConstraintManager::validateGlobalConstraint(const ConstraintDefinition& constraint, 
                                                const std::string& value,
                                                const std::map<std::string, std::string>& context) const {
    return constraint.validator(value, context);
}

std::function<bool(const std::string&, const std::map<std::string, std::string>&)> ConstraintManager::createDefaultValidator(
    ConstraintType type, const std::string& target) const {
    
    switch (type) {
        case ConstraintType::PRECISE:
            return [](const std::string& value, const std::map<std::string, std::string>&) {
                return !value.empty();
            };
        case ConstraintType::TYPE:
            return [](const std::string& value, const std::map<std::string, std::string>&) {
                return !value.empty();
            };
        case ConstraintType::GLOBAL:
            return [](const std::string& value, const std::map<std::string, std::string>&) {
                return !value.empty();
            };
        default:
            return [](const std::string&, const std::map<std::string, std::string>&) {
                return true;
            };
    }
}

bool ConstraintManager::validateConstraintName(const std::string& name) const {
    if (name.empty()) {
        return false;
    }
    
    std::regex nameRegex(R"([a-zA-Z_][a-zA-Z0-9_]*)");
    return std::regex_match(name, nameRegex);
}

bool ConstraintManager::validateConstraintTarget(const std::string& target) const {
    if (target.empty()) {
        return false;
    }
    
    std::regex targetRegex(R"([a-zA-Z_][a-zA-Z0-9_.]*)");
    return std::regex_match(target, targetRegex);
}

} // namespace CHTL