#pragma once

#include "CHTLNode/ConstraintNode.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace CHTL {

class ConstraintManager {
public:
    ConstraintManager();
    virtual ~ConstraintManager() = default;
    
    // 约束管理
    void addConstraint(std::shared_ptr<ConstraintNode> constraint);
    void removeConstraint(std::shared_ptr<ConstraintNode> constraint);
    std::vector<std::shared_ptr<ConstraintNode>> getConstraints() const { return constraints; }
    
    // 约束查找
    std::vector<std::shared_ptr<ConstraintNode>> getConstraintsByType(ConstraintType type) const;
    std::vector<std::shared_ptr<ConstraintNode>> getConstraintsByTarget(ConstraintTarget target) const;
    std::vector<std::shared_ptr<ConstraintNode>> getConstraintsByScope(const std::string& scope) const;
    
    // 约束验证
    bool validateTarget(const std::string& target, ConstraintTarget targetType) const;
    bool validateElement(const std::string& elementName) const;
    bool validateAttribute(const std::string& attributeName) const;
    bool validateStyle(const std::string& styleName) const;
    bool validateScript(const std::string& scriptName) const;
    bool validateTemplate(const std::string& templateName) const;
    bool validateCustom(const std::string& customName) const;
    bool validateNamespace(const std::string& namespaceName) const;
    
    // 约束应用
    std::vector<std::shared_ptr<ConstraintNode>> getApplicableConstraints(const std::string& target, 
                                                                          ConstraintTarget targetType) const;
    bool hasConstraint(const std::string& target, ConstraintTarget targetType) const;
    bool isAllowed(const std::string& target, ConstraintTarget targetType) const;
    bool isForbidden(const std::string& target, ConstraintTarget targetType) const;
    
    // 约束优先级
    void setConstraintPriority(std::shared_ptr<ConstraintNode> constraint, int priority);
    int getConstraintPriority(std::shared_ptr<ConstraintNode> constraint) const;
    void sortConstraintsByPriority();
    
    // 约束作用域
    void setGlobalScope(const std::string& scope) { globalScope = scope; }
    const std::string& getGlobalScope() const { return globalScope; }
    
    void addScopeConstraint(const std::string& scope, std::shared_ptr<ConstraintNode> constraint);
    void removeScopeConstraint(const std::string& scope, std::shared_ptr<ConstraintNode> constraint);
    std::vector<std::shared_ptr<ConstraintNode>> getScopeConstraints(const std::string& scope) const;
    
    // 约束继承
    void enableInheritance(bool enable) { inheritanceEnabled = enable; }
    bool isInheritanceEnabled() const { return inheritanceEnabled; }
    
    void inheritConstraints(const std::string& fromScope, const std::string& toScope);
    void inheritAllConstraints(const std::string& toScope);
    
    // 约束合并
    void mergeConstraints(const std::string& targetScope, const std::string& sourceScope);
    void mergeAllConstraints(const std::string& targetScope);
    
    // 约束冲突检测
    bool hasConflicts() const;
    std::vector<std::string> getConflicts() const;
    bool resolveConflict(const std::string& conflict, const std::string& resolution);
    
    // 约束统计
    struct ConstraintStats {
        size_t totalConstraints;
        size_t preciseConstraints;
        size_t typeConstraints;
        size_t globalConstraints;
        size_t enabledConstraints;
        size_t disabledConstraints;
        size_t conflicts;
        size_t scopeConstraints;
    };
    
    ConstraintStats getStats() const { return stats; }
    void resetStats();
    
    // 约束导出
    std::string exportConstraints() const;
    std::string exportConstraintsByScope(const std::string& scope) const;
    std::string exportConstraintsByType(ConstraintType type) const;
    
    // 约束导入
    void importConstraints(const std::string& constraintsData);
    void importConstraintsFromFile(const std::string& filePath);
    
    // 调试
    void enableDebugMode(bool enable) { debugMode = enable; }
    bool isDebugMode() const { return debugMode; }
    std::string getDebugInfo() const;
    
private:
    std::vector<std::shared_ptr<ConstraintNode>> constraints;
    std::map<std::string, std::vector<std::shared_ptr<ConstraintNode>>> scopeConstraints;
    std::string globalScope;
    bool inheritanceEnabled;
    bool debugMode;
    ConstraintStats stats;
    
    // 内部方法
    bool isConstraintApplicable(std::shared_ptr<ConstraintNode> constraint, 
                               const std::string& target, ConstraintTarget targetType) const;
    std::vector<std::shared_ptr<ConstraintNode>> getConstraintsForTarget(const std::string& target, 
                                                                         ConstraintTarget targetType) const;
    bool evaluateConstraint(std::shared_ptr<ConstraintNode> constraint, 
                           const std::string& target) const;
    
    // 冲突检测
    std::vector<std::string> findConflicts() const;
    bool hasConstraintConflict(std::shared_ptr<ConstraintNode> c1, 
                              std::shared_ptr<ConstraintNode> c2) const;
    
    // 优先级管理
    void sortConstraintsByPriority(std::vector<std::shared_ptr<ConstraintNode>>& constraints) const;
    
    // 调试
    void debugLog(const std::string& message) const;
    void updateStats();
};

} // namespace CHTL