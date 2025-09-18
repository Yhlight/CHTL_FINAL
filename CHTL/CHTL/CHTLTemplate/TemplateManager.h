#ifndef TEMPLATE_MANAGER_H
#define TEMPLATE_MANAGER_H

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/TemplateNode.h"
#include "../CHTLNode/CustomNode.h"
#include <string>
#include <unordered_map>
#include <memory>

namespace CHTL {

/**
 * 模板管理器
 * 负责管理模板的注册、查找和使用
 */
class TemplateManager {
public:
    TemplateManager();
    virtual ~TemplateManager() = default;
    
    // 注册模板
    void registerTemplate(std::shared_ptr<TemplateNode> templateNode);
    void registerCustom(std::shared_ptr<CustomNode> customNode);
    
    // 查找模板
    std::shared_ptr<TemplateNode> findTemplate(const std::string& name) const;
    std::shared_ptr<CustomNode> findCustom(const std::string& name) const;
    
    // 使用模板
    std::shared_ptr<BaseNode> useTemplate(const std::string& name, 
                                         const std::map<std::string, std::string>& parameters = {}) const;
    std::shared_ptr<BaseNode> useCustom(const std::string& name, 
                                       const std::map<std::string, std::string>& parameters = {}) const;
    
    // 模板继承
    std::shared_ptr<BaseNode> inheritTemplate(std::shared_ptr<TemplateNode> child, 
                                             std::shared_ptr<TemplateNode> parent) const;
    std::shared_ptr<BaseNode> inheritCustom(std::shared_ptr<CustomNode> child, 
                                           std::shared_ptr<CustomNode> parent) const;
    
    // 清空所有模板
    void clear();
    
    // 获取模板数量
    size_t getTemplateCount() const { return m_templates.size(); }
    size_t getCustomCount() const { return m_customs.size(); }

private:
    std::unordered_map<std::string, std::shared_ptr<TemplateNode>> m_templates;
    std::unordered_map<std::string, std::shared_ptr<CustomNode>> m_customs;
    
    // 克隆节点并替换参数
    std::shared_ptr<BaseNode> cloneAndReplace(std::shared_ptr<BaseNode> node, 
                                             const std::map<std::string, std::string>& parameters) const;
    
    // 合并样式属性
    void mergeStyleProperties(std::shared_ptr<BaseNode> target, 
                             std::shared_ptr<BaseNode> source) const;
};

} // namespace CHTL

#endif // TEMPLATE_MANAGER_H