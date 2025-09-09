#pragma once

#include <CHTL/CHTLNode/TemplateStyleNode.hpp>
#include <CHTL/CHTLNode/TemplateElementNode.hpp>
#include <CHTL/CHTLNode/TemplateVarNode.hpp>
#include <string>
#include <map>
#include <memory>
#include <vector>

namespace CHTL {

/**
 * @brief 模板管理器
 * 
 * 根据 CHTL.md 文档第 259-388 行实现
 * 统一管理所有类型的模板
 */
class TemplateManager {
public:
    // 单例模式
    static TemplateManager& getInstance();
    
    // 禁止拷贝和赋值
    TemplateManager(const TemplateManager&) = delete;
    TemplateManager& operator=(const TemplateManager&) = delete;
    
    // 样式组模板管理
    void addStyleTemplate(const std::string& name, std::shared_ptr<TemplateStyleNode> template_node);
    std::shared_ptr<TemplateStyleNode> getStyleTemplate(const std::string& name) const;
    bool hasStyleTemplate(const std::string& name) const;
    std::vector<std::string> getAllStyleTemplateNames() const;
    
    // 元素模板管理
    void addElementTemplate(const std::string& name, std::shared_ptr<TemplateElementNode> template_node);
    std::shared_ptr<TemplateElementNode> getElementTemplate(const std::string& name) const;
    bool hasElementTemplate(const std::string& name) const;
    std::vector<std::string> getAllElementTemplateNames() const;
    
    // 变量组模板管理
    void addVarTemplate(const std::string& name, std::shared_ptr<TemplateVarNode> template_node);
    std::shared_ptr<TemplateVarNode> getVarTemplate(const std::string& name) const;
    bool hasVarTemplate(const std::string& name) const;
    std::vector<std::string> getAllVarTemplateNames() const;
    
    // 模板继承处理
    void resolveInheritance();
    
    // 模板引用解析
    std::string resolveStyleTemplate(const std::string& templateName) const;
    std::string resolveElementTemplate(const std::string& templateName) const;
    std::string resolveVarTemplate(const std::string& templateName, const std::string& variableName) const;
    
    // 清理所有模板
    void clear();
    
    // 重置到初始状态
    void reset();
    
    // 获取模板统计信息
    size_t getStyleTemplateCount() const { return styleTemplates_.size(); }
    size_t getElementTemplateCount() const { return elementTemplates_.size(); }
    size_t getVarTemplateCount() const { return varTemplates_.size(); }
    
private:
    TemplateManager() = default;
    ~TemplateManager() = default;
    
    // 样式组模板存储
    std::map<std::string, std::shared_ptr<TemplateStyleNode>> styleTemplates_;
    
    // 元素模板存储
    std::map<std::string, std::shared_ptr<TemplateElementNode>> elementTemplates_;
    
    // 变量组模板存储
    std::map<std::string, std::shared_ptr<TemplateVarNode>> varTemplates_;
    
    // 解析样式组模板继承
    void resolveStyleTemplateInheritance(const std::string& templateName);
    
    // 解析元素模板继承
    void resolveElementTemplateInheritance(const std::string& templateName);
    
    // 解析变量组模板继承
    void resolveVarTemplateInheritance(const std::string& templateName);
};

} // namespace CHTL