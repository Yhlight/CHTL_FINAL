#pragma once

#include <string>
#include <map>
#include <memory>
#include <vector>
#include <set>
#include <functional>
#include "CHTLNode/BaseNode.hpp"

namespace CHTL {

/**
 * @brief 约束管理器
 * 
 * 管理精确约束、类型约束、全局约束
 */
class ConstraintManager {
public:
    enum class ConstraintType {
        PRECISE,        // 精确约束
        TYPE,           // 类型约束
        GLOBAL          // 全局约束
    };
    
    enum class ConstraintScope {
        ELEMENT,        // 元素约束
        STYLE,          // 样式约束
        SCRIPT,         // 脚本约束
        TEMPLATE,       // 模板约束
        CUSTOM,         // 自定义约束
        NAMESPACE       // 命名空间约束
    };
    
    struct ConstraintDefinition {
        std::string name;
        ConstraintType type;
        ConstraintScope scope;
        std::string target;
        std::map<std::string, std::string> parameters;
        std::function<bool(const std::string&, const std::map<std::string, std::string>&)> validator;
        std::string errorMessage;
        bool isActive;
    };
    
    // 单例模式
    static ConstraintManager& getInstance();
    
    // 添加约束
    void addConstraint(const std::string& name, ConstraintType type, ConstraintScope scope,
                      const std::string& target, 
                      std::function<bool(const std::string&, const std::map<std::string, std::string>&)> validator,
                      const std::string& errorMessage = "");
    
    // 添加精确约束
    void addPreciseConstraint(const std::string& name, ConstraintScope scope,
                             const std::string& target, const std::string& expectedValue,
                             const std::string& errorMessage = "");
    
    // 添加类型约束
    void addTypeConstraint(const std::string& name, ConstraintScope scope,
                          const std::string& target, const std::string& expectedType,
                          const std::string& errorMessage = "");
    
    // 添加全局约束
    void addGlobalConstraint(const std::string& name, ConstraintScope scope,
                            const std::string& target, 
                            std::function<bool(const std::string&, const std::map<std::string, std::string>&)> validator,
                            const std::string& errorMessage = "");
    
    // 获取约束定义
    std::shared_ptr<ConstraintDefinition> getConstraint(const std::string& name) const;
    
    // 检查约束是否存在
    bool hasConstraint(const std::string& name) const;
    
    // 验证约束
    bool validateConstraint(const std::string& name, const std::string& value, 
                           const std::map<std::string, std::string>& context = {}) const;
    
    // 验证所有约束
    std::vector<std::string> validateAllConstraints(const std::string& value, 
                                                   ConstraintScope scope,
                                                   const std::map<std::string, std::string>& context = {}) const;
    
    // 获取约束错误信息
    std::string getConstraintErrorMessage(const std::string& name) const;
    
    // 激活约束
    void activateConstraint(const std::string& name);
    
    // 停用约束
    void deactivateConstraint(const std::string& name);
    
    // 检查约束是否激活
    bool isConstraintActive(const std::string& name) const;
    
    // 获取所有约束名称
    std::vector<std::string> getAllConstraintNames() const;
    
    // 获取指定类型的约束名称
    std::vector<std::string> getConstraintNamesByType(ConstraintType type) const;
    
    // 获取指定作用域的约束名称
    std::vector<std::string> getConstraintNamesByScope(ConstraintScope scope) const;
    
    // 清除约束
    void clearConstraint(const std::string& name);
    
    // 清除所有约束
    void clear();
    
    // 移除约束
    void removeConstraint(const std::string& name);
    
    // 设置约束参数
    void setConstraintParameter(const std::string& name, const std::string& paramName, 
                               const std::string& paramValue);
    
    // 获取约束参数
    std::string getConstraintParameter(const std::string& name, const std::string& paramName) const;
    
    // 获取所有约束参数
    std::map<std::string, std::string> getConstraintParameters(const std::string& name) const;
    
    // 添加约束组
    void addConstraintGroup(const std::string& groupName, const std::vector<std::string>& constraintNames);
    
    // 获取约束组
    std::vector<std::string> getConstraintGroup(const std::string& groupName) const;
    
    // 验证约束组
    std::vector<std::string> validateConstraintGroup(const std::string& groupName, 
                                                    const std::string& value,
                                                    const std::map<std::string, std::string>& context = {}) const;
    
    // 获取所有约束组名称
    std::vector<std::string> getAllConstraintGroupNames() const;
    
    // 清除约束组
    void clearConstraintGroup(const std::string& groupName);
    
private:
    ConstraintManager() = default;
    ~ConstraintManager() = default;
    ConstraintManager(const ConstraintManager&) = delete;
    ConstraintManager& operator=(const ConstraintManager&) = delete;
    
    std::map<std::string, std::shared_ptr<ConstraintDefinition>> constraints_;
    std::map<std::string, std::vector<std::string>> constraintGroups_;
    
    // 验证精确约束
    bool validatePreciseConstraint(const ConstraintDefinition& constraint, 
                                  const std::string& value) const;
    
    // 验证类型约束
    bool validateTypeConstraint(const ConstraintDefinition& constraint, 
                               const std::string& value) const;
    
    // 验证全局约束
    bool validateGlobalConstraint(const ConstraintDefinition& constraint, 
                                 const std::string& value,
                                 const std::map<std::string, std::string>& context) const;
    
    // 创建默认验证器
    std::function<bool(const std::string&, const std::map<std::string, std::string>&)> createDefaultValidator(
        ConstraintType type, const std::string& target) const;
    
    // 验证约束名称
    bool validateConstraintName(const std::string& name) const;
    
    // 验证约束目标
    bool validateConstraintTarget(const std::string& target) const;
};

} // namespace CHTL