#pragma once

#include <string>
#include <map>
#include <memory>
#include <vector>
#include "CHTLNode/BaseNode.hpp"

namespace CHTL {

/**
 * @brief 自定义系统管理器
 * 
 * 管理自定义样式组、元素、变量组及其特例化操作
 */
class CustomManager {
public:
    enum class CustomType {
        STYLE_GROUP,    // 自定义样式组
        ELEMENT,        // 自定义元素
        VARIABLE_GROUP  // 自定义变量组
    };
    
    struct CustomDefinition {
        CustomType type;
        std::string name;
        std::shared_ptr<BaseNode> definition;
        std::vector<std::string> deleteOperations;
        std::vector<std::shared_ptr<BaseNode>> insertOperations;
        std::map<std::string, std::string> replaceOperations;
    };
    
    // 单例模式
    static CustomManager& getInstance();
    
    // 添加自定义定义
    void addCustom(const std::string& name, CustomType type, 
                  std::shared_ptr<BaseNode> definition);
    
    // 获取自定义定义
    std::shared_ptr<CustomDefinition> getCustom(const std::string& name) const;
    
    // 检查自定义是否存在
    bool hasCustom(const std::string& name) const;
    
    // 实例化自定义
    std::shared_ptr<BaseNode> instantiateCustom(const std::string& name, 
                                               const std::map<std::string, std::string>& parameters = {}) const;
    
    // 添加删除操作
    void addDeleteOperation(const std::string& name, const std::string& operation);
    
    // 添加插入操作
    void addInsertOperation(const std::string& name, std::shared_ptr<BaseNode> operation);
    
    // 添加替换操作
    void addReplaceOperation(const std::string& name, const std::string& selector, 
                           const std::string& replacement);
    
    // 获取所有自定义名称
    std::vector<std::string> getAllCustomNames() const;
    
    // 获取指定类型的所有自定义名称
    std::vector<std::string> getCustomNamesByType(CustomType type) const;
    
    // 清除所有自定义
    void clear();
    
    // 移除自定义
    void removeCustom(const std::string& name);
    
private:
    CustomManager() = default;
    ~CustomManager() = default;
    CustomManager(const CustomManager&) = delete;
    CustomManager& operator=(const CustomManager&) = delete;
    
    std::map<std::string, std::shared_ptr<CustomDefinition>> customs_;
    
    // 实例化自定义样式组
    std::shared_ptr<BaseNode> instantiateCustomStyleGroup(
        const CustomDefinition& customDef,
        const std::map<std::string, std::string>& parameters) const;
    
    // 实例化自定义元素
    std::shared_ptr<BaseNode> instantiateCustomElement(
        const CustomDefinition& customDef,
        const std::map<std::string, std::string>& parameters) const;
    
    // 实例化自定义变量组
    std::shared_ptr<BaseNode> instantiateCustomVariableGroup(
        const CustomDefinition& customDef,
        const std::map<std::string, std::string>& parameters) const;
    
    // 应用删除操作
    void applyDeleteOperations(std::shared_ptr<BaseNode> node, 
                              const std::vector<std::string>& deleteOps) const;
    
    // 应用插入操作
    void applyInsertOperations(std::shared_ptr<BaseNode> node, 
                              const std::vector<std::shared_ptr<BaseNode>>& insertOps) const;
    
    // 应用替换操作
    void applyReplaceOperations(std::shared_ptr<BaseNode> node, 
                               const std::map<std::string, std::string>& replaceOps) const;
    
    // 深度复制节点
    std::shared_ptr<BaseNode> deepCopyNode(std::shared_ptr<BaseNode> node) const;
    
    // 替换模板参数
    std::string replaceParameters(const std::string& content, 
                                const std::map<std::string, std::string>& parameters) const;
};

} // namespace CHTL