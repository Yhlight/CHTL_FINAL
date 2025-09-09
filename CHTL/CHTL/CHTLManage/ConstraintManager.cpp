#include "ConstraintManager.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <fstream>

namespace CHTL {

ConstraintManager::ConstraintManager() 
    : inheritanceEnabled(true), debugMode(false) {
    resetStats();
}

void ConstraintManager::addConstraint(std::shared_ptr<ConstraintNode> constraint) {
    if (!constraint) {
        return;
    }
    
    constraints.push_back(constraint);
    
    // 添加到作用域约束
    std::string scope = constraint->getScope();
    if (scope.empty()) {
        scope = globalScope;
    }
    scopeConstraints[scope].push_back(constraint);
    
    updateStats();
    debugLog("Added constraint: " + constraint->getConstraintExpression());
}

void ConstraintManager::removeConstraint(std::shared_ptr<ConstraintNode> constraint) {
    if (!constraint) {
        return;
    }
    
    auto it = std::find(constraints.begin(), constraints.end(), constraint);
    if (it != constraints.end()) {
        constraints.erase(it);
        
        // 从作用域约束中移除
        std::string scope = constraint->getScope();
        if (scope.empty()) {
            scope = globalScope;
        }
        
        auto scopeIt = scopeConstraints.find(scope);
        if (scopeIt != scopeConstraints.end()) {
            auto scopeConstraintIt = std::find(scopeIt->second.begin(), scopeIt->second.end(), constraint);
            if (scopeConstraintIt != scopeIt->second.end()) {
                scopeIt->second.erase(scopeConstraintIt);
            }
        }
        
        updateStats();
        debugLog("Removed constraint: " + constraint->getConstraintExpression());
    }
}

std::vector<std::shared_ptr<ConstraintNode>> ConstraintManager::getConstraintsByType(ConstraintType type) const {
    std::vector<std::shared_ptr<ConstraintNode>> result;
    
    for (const auto& constraint : constraints) {
        if (constraint->getConstraintType() == type) {
            result.push_back(constraint);
        }
    }
    
    return result;
}

std::vector<std::shared_ptr<ConstraintNode>> ConstraintManager::getConstraintsByTarget(ConstraintTarget target) const {
    std::vector<std::shared_ptr<ConstraintNode>> result;
    
    for (const auto& constraint : constraints) {
        if (constraint->getTarget() == target || constraint->getTarget() == ConstraintTarget::ALL) {
            result.push_back(constraint);
        }
    }
    
    return result;
}

std::vector<std::shared_ptr<ConstraintNode>> ConstraintManager::getConstraintsByScope(const std::string& scope) const {
    auto it = scopeConstraints.find(scope);
    if (it != scopeConstraints.end()) {
        return it->second;
    }
    return {};
}

bool ConstraintManager::validateTarget(const std::string& target, ConstraintTarget targetType) const {
    auto applicableConstraints = getApplicableConstraints(target, targetType);
    
    for (const auto& constraint : applicableConstraints) {
        if (!constraint->validate(target)) {
            debugLog("Validation failed for target: " + target + " with constraint: " + constraint->getConstraintExpression());
            return false;
        }
    }
    
    return true;
}

bool ConstraintManager::validateElement(const std::string& elementName) const {
    return validateTarget(elementName, ConstraintTarget::ELEMENT);
}

bool ConstraintManager::validateAttribute(const std::string& attributeName) const {
    return validateTarget(attributeName, ConstraintTarget::ATTRIBUTE);
}

bool ConstraintManager::validateStyle(const std::string& styleName) const {
    return validateTarget(styleName, ConstraintTarget::STYLE);
}

bool ConstraintManager::validateScript(const std::string& scriptName) const {
    return validateTarget(scriptName, ConstraintTarget::SCRIPT);
}

bool ConstraintManager::validateTemplate(const std::string& templateName) const {
    return validateTarget(templateName, ConstraintTarget::TEMPLATE);
}

bool ConstraintManager::validateCustom(const std::string& customName) const {
    return validateTarget(customName, ConstraintTarget::CUSTOM);
}

bool ConstraintManager::validateNamespace(const std::string& namespaceName) const {
    return validateTarget(namespaceName, ConstraintTarget::NAMESPACE);
}

std::vector<std::shared_ptr<ConstraintNode>> ConstraintManager::getApplicableConstraints(const std::string& target, 
                                                                                        ConstraintTarget targetType) const {
    std::vector<std::shared_ptr<ConstraintNode>> result;
    
    for (const auto& constraint : constraints) {
        if (isConstraintApplicable(constraint, target, targetType)) {
            result.push_back(constraint);
        }
    }
    
    // 按优先级排序
    sortConstraintsByPriority(result);
    
    return result;
}

bool ConstraintManager::hasConstraint(const std::string& target, ConstraintTarget targetType) const {
    return !getApplicableConstraints(target, targetType).empty();
}

bool ConstraintManager::isAllowed(const std::string& target, ConstraintTarget targetType) const {
    return validateTarget(target, targetType);
}

bool ConstraintManager::isForbidden(const std::string& target, ConstraintTarget targetType) const {
    return !isAllowed(target, targetType);
}

void ConstraintManager::setConstraintPriority(std::shared_ptr<ConstraintNode> constraint, int priority) {
    if (constraint) {
        constraint->setPriority(priority);
        sortConstraintsByPriority();
    }
}

int ConstraintManager::getConstraintPriority(std::shared_ptr<ConstraintNode> constraint) const {
    if (constraint) {
        return constraint->getPriority();
    }
    return 0;
}

void ConstraintManager::sortConstraintsByPriority() {
    sortConstraintsByPriority(constraints);
    
    for (auto& scopeConstraint : scopeConstraints) {
        sortConstraintsByPriority(scopeConstraint.second);
    }
}

void ConstraintManager::addScopeConstraint(const std::string& scope, std::shared_ptr<ConstraintNode> constraint) {
    if (constraint) {
        scopeConstraints[scope].push_back(constraint);
        updateStats();
        debugLog("Added constraint to scope: " + scope);
    }
}

void ConstraintManager::removeScopeConstraint(const std::string& scope, std::shared_ptr<ConstraintNode> constraint) {
    auto it = scopeConstraints.find(scope);
    if (it != scopeConstraints.end()) {
        auto constraintIt = std::find(it->second.begin(), it->second.end(), constraint);
        if (constraintIt != it->second.end()) {
            it->second.erase(constraintIt);
            updateStats();
            debugLog("Removed constraint from scope: " + scope);
        }
    }
}

std::vector<std::shared_ptr<ConstraintNode>> ConstraintManager::getScopeConstraints(const std::string& scope) const {
    return getConstraintsByScope(scope);
}

void ConstraintManager::inheritConstraints(const std::string& fromScope, const std::string& toScope) {
    if (!inheritanceEnabled) {
        return;
    }
    
    auto fromConstraints = getConstraintsByScope(fromScope);
    for (const auto& constraint : fromConstraints) {
        auto cloned = constraint->clone();
        cloned->setScope(toScope);
        addConstraint(cloned);
    }
    
    debugLog("Inherited constraints from " + fromScope + " to " + toScope);
}

void ConstraintManager::inheritAllConstraints(const std::string& toScope) {
    for (const auto& scopeConstraint : scopeConstraints) {
        if (scopeConstraint.first != toScope) {
            inheritConstraints(scopeConstraint.first, toScope);
        }
    }
}

void ConstraintManager::mergeConstraints(const std::string& targetScope, const std::string& sourceScope) {
    auto sourceConstraints = getConstraintsByScope(sourceScope);
    for (const auto& constraint : sourceConstraints) {
        constraint->setScope(targetScope);
        addConstraint(constraint);
    }
    
    debugLog("Merged constraints from " + sourceScope + " to " + targetScope);
}

void ConstraintManager::mergeAllConstraints(const std::string& targetScope) {
    for (const auto& scopeConstraint : scopeConstraints) {
        if (scopeConstraint.first != targetScope) {
            mergeConstraints(targetScope, scopeConstraint.first);
        }
    }
}

bool ConstraintManager::hasConflicts() const {
    return !findConflicts().empty();
}

std::vector<std::string> ConstraintManager::getConflicts() const {
    return findConflicts();
}

bool ConstraintManager::resolveConflict(const std::string& conflict, const std::string& resolution) {
    // 简化实现
    debugLog("Resolving conflict: " + conflict + " with resolution: " + resolution);
    return true;
}

std::string ConstraintManager::exportConstraints() const {
    std::ostringstream oss;
    
    for (const auto& constraint : constraints) {
        oss << "constraint " << ConstraintNode::constraintTypeToString(constraint->getConstraintType()) 
            << " " << ConstraintNode::constraintTargetToString(constraint->getTarget()) 
            << " \"" << constraint->getConstraintExpression() << "\"";
        
        if (!constraint->getScope().empty()) {
            oss << " scope \"" << constraint->getScope() << "\"";
        }
        
        if (constraint->getPriority() != 0) {
            oss << " priority " << constraint->getPriority();
        }
        
        oss << ";\n";
    }
    
    return oss.str();
}

std::string ConstraintManager::exportConstraintsByScope(const std::string& scope) const {
    std::ostringstream oss;
    
    auto scopeConstraints = getConstraintsByScope(scope);
    for (const auto& constraint : scopeConstraints) {
        oss << "constraint " << ConstraintNode::constraintTypeToString(constraint->getConstraintType()) 
            << " " << ConstraintNode::constraintTargetToString(constraint->getTarget()) 
            << " \"" << constraint->getConstraintExpression() << "\";\n";
    }
    
    return oss.str();
}

std::string ConstraintManager::exportConstraintsByType(ConstraintType type) const {
    std::ostringstream oss;
    
    auto typeConstraints = getConstraintsByType(type);
    for (const auto& constraint : typeConstraints) {
        oss << "constraint " << ConstraintNode::constraintTypeToString(constraint->getConstraintType()) 
            << " " << ConstraintNode::constraintTargetToString(constraint->getTarget()) 
            << " \"" << constraint->getConstraintExpression() << "\";\n";
    }
    
    return oss.str();
}

void ConstraintManager::importConstraints(const std::string& constraintsData) {
    // 简化实现：解析约束数据并添加约束
    debugLog("Importing constraints from data");
}

void ConstraintManager::importConstraintsFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        debugLog("Failed to open constraints file: " + filePath);
        return;
    }
    
    std::ostringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    importConstraints(buffer.str());
    debugLog("Imported constraints from file: " + filePath);
}

std::string ConstraintManager::getDebugInfo() const {
    std::ostringstream oss;
    oss << "ConstraintManager Debug Info:\n";
    oss << "  Total Constraints: " << stats.totalConstraints << "\n";
    oss << "  Precise Constraints: " << stats.preciseConstraints << "\n";
    oss << "  Type Constraints: " << stats.typeConstraints << "\n";
    oss << "  Global Constraints: " << stats.globalConstraints << "\n";
    oss << "  Enabled Constraints: " << stats.enabledConstraints << "\n";
    oss << "  Disabled Constraints: " << stats.disabledConstraints << "\n";
    oss << "  Conflicts: " << stats.conflicts << "\n";
    oss << "  Scope Constraints: " << stats.scopeConstraints << "\n";
    oss << "  Global Scope: " << globalScope << "\n";
    oss << "  Inheritance Enabled: " << (inheritanceEnabled ? "true" : "false") << "\n";
    return oss.str();
}

bool ConstraintManager::isConstraintApplicable(std::shared_ptr<ConstraintNode> constraint, 
                                              const std::string& target, ConstraintTarget targetType) const {
    if (!constraint->isConstraintEnabled()) {
        return false;
    }
    
    if (constraint->getTarget() != targetType && constraint->getTarget() != ConstraintTarget::ALL) {
        return false;
    }
    
    return constraint->appliesTo(target);
}

std::vector<std::shared_ptr<ConstraintNode>> ConstraintManager::getConstraintsForTarget(const std::string& target, 
                                                                                       ConstraintTarget targetType) const {
    return getApplicableConstraints(target, targetType);
}

bool ConstraintManager::evaluateConstraint(std::shared_ptr<ConstraintNode> constraint, 
                                          const std::string& target) const {
    return constraint->evaluateConstraint(target);
}

std::vector<std::string> ConstraintManager::findConflicts() const {
    std::vector<std::string> conflicts;
    
    for (size_t i = 0; i < constraints.size(); ++i) {
        for (size_t j = i + 1; j < constraints.size(); ++j) {
            if (hasConstraintConflict(constraints[i], constraints[j])) {
                conflicts.push_back("Conflict between constraints: " + 
                                  constraints[i]->getConstraintExpression() + " and " + 
                                  constraints[j]->getConstraintExpression());
            }
        }
    }
    
    return conflicts;
}

bool ConstraintManager::hasConstraintConflict(std::shared_ptr<ConstraintNode> c1, 
                                             std::shared_ptr<ConstraintNode> c2) const {
    // 简化实现：检查是否有相同的约束表达式但不同的目标
    return c1->getConstraintExpression() == c2->getConstraintExpression() &&
           c1->getTarget() != c2->getTarget();
}

void ConstraintManager::sortConstraintsByPriority(std::vector<std::shared_ptr<ConstraintNode>>& constraints) const {
    std::sort(constraints.begin(), constraints.end(), 
        [](const std::shared_ptr<ConstraintNode>& a, const std::shared_ptr<ConstraintNode>& b) {
            return a->getPriority() > b->getPriority();
        });
}

void ConstraintManager::debugLog(const std::string& message) const {
    if (debugMode) {
        std::cout << "[ConstraintManager] " << message << std::endl;
    }
}

void ConstraintManager::updateStats() {
    stats.totalConstraints = constraints.size();
    stats.preciseConstraints = 0;
    stats.typeConstraints = 0;
    stats.globalConstraints = 0;
    stats.enabledConstraints = 0;
    stats.disabledConstraints = 0;
    stats.conflicts = findConflicts().size();
    stats.scopeConstraints = scopeConstraints.size();
    
    for (const auto& constraint : constraints) {
        switch (constraint->getConstraintType()) {
            case ConstraintType::PRECISE:
                stats.preciseConstraints++;
                break;
            case ConstraintType::TYPE:
                stats.typeConstraints++;
                break;
            case ConstraintType::GLOBAL:
                stats.globalConstraints++;
                break;
        }
        
        if (constraint->isConstraintEnabled()) {
            stats.enabledConstraints++;
        } else {
            stats.disabledConstraints++;
        }
    }
}

void ConstraintManager::resetStats() {
    stats = ConstraintStats{};
}

} // namespace CHTL