#pragma once

#include <string>
#include <map>
#include <memory>
#include <vector>
#include "CHTLNode/BaseNode.hpp"

namespace CHTL {

/**
 * @brief 模板管理器
 * 
 * 管理所有的模板定义和实例化
 */
class TemplateManager {
public:
    enum class TemplateType {
        STYLE_GROUP,    // 样式组模板
        ELEMENT,        // 元素模板
        VARIABLE_GROUP  // 变量组模板
    };
    
    struct TemplateDefinition {
        TemplateType type;
        std::string name;
        std::shared_ptr<BaseNode> definition;
        std::map<std::string, std::string> parameters;
    };
    
    // 单例模式
    static TemplateManager& getInstance();
    
    // 添加模板定义
    void addTemplate(const std::string& name, TemplateType type, 
                    std::shared_ptr<BaseNode> definition);
    
    // 获取模板定义
    std::shared_ptr<TemplateDefinition> getTemplate(const std::string& name) const;
    
    // 检查模板是否存在
    bool hasTemplate(const std::string& name) const;
    
    // 实例化模板
    std::shared_ptr<BaseNode> instantiateTemplate(const std::string& name, 
                                                 const std::map<std::string, std::string>& parameters = {}) const;
    
    // 获取所有模板名称
    std::vector<std::string> getAllTemplateNames() const;
    
    // 获取指定类型的所有模板名称
    std::vector<std::string> getTemplateNamesByType(TemplateType type) const;
    
    // 清除所有模板
    void clear();
    
    // 移除模板
    void removeTemplate(const std::string& name);
    
private:
    TemplateManager() = default;
    ~TemplateManager() = default;
    TemplateManager(const TemplateManager&) = delete;
    TemplateManager& operator=(const TemplateManager&) = delete;
    
    std::map<std::string, std::shared_ptr<TemplateDefinition>> templates_;
    
    // 实例化样式组模板
    std::shared_ptr<BaseNode> instantiateStyleGroupTemplate(
        const TemplateDefinition& templateDef,
        const std::map<std::string, std::string>& parameters) const;
    
    // 实例化元素模板
    std::shared_ptr<BaseNode> instantiateElementTemplate(
        const TemplateDefinition& templateDef,
        const std::map<std::string, std::string>& parameters) const;
    
    // 实例化变量组模板
    std::shared_ptr<BaseNode> instantiateVariableGroupTemplate(
        const TemplateDefinition& templateDef,
        const std::map<std::string, std::string>& parameters) const;
    
    // 替换模板参数
    std::string replaceParameters(const std::string& content, 
                                const std::map<std::string, std::string>& parameters) const;
    
    // 深度复制节点
    std::shared_ptr<BaseNode> deepCopyNode(std::shared_ptr<BaseNode> node) const;
};

} // namespace CHTL