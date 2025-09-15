#pragma once

#include "CHTL/CHTLNode.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {

// 使用CHTLNode.h中定义的TemplateType

/**
 * 模板信息
 */
struct TemplateInfo {
    CHTLNode::TemplateType type;
    std::string name;
    std::string content;
    std::map<std::string, std::string> properties;
    std::vector<std::shared_ptr<CHTLNode>> children;
    
    TemplateInfo() = default;
    TemplateInfo(CHTLNode::TemplateType t, const std::string& n, const std::string& c)
        : type(t), name(n), content(c) {}
    
    // 添加赋值操作符
    TemplateInfo& operator=(const TemplateInfo& other) = default;
};

/**
 * 模板解析器
 */
class TemplateParser {
public:
    TemplateParser();
    ~TemplateParser();
    
    // 解析模板
    std::vector<TemplateInfo> parseTemplates(const std::string& content);
    TemplateInfo parseTemplate(const std::string& template_content);
    
    // 模板应用
    std::string applyStyleTemplate(const std::string& content, const std::string& template_name);
    std::string applyElementTemplate(const std::string& content, const std::string& template_name);
    std::string applyVariableTemplate(const std::string& content, const std::string& template_name);
    
    // 模板管理
    void addTemplate(const TemplateInfo& template_info);
    void removeTemplate(const std::string& name);
    TemplateInfo getTemplate(const std::string& name) const;
    std::vector<std::string> getTemplateNames() const;
    
    // 清理
    void clear();
    
private:
    std::map<std::string, TemplateInfo> templates_;
    
    // 解析辅助函数
    CHTLNode::TemplateType parseTemplateType(const std::string& content);
    std::string extractTemplateName(const std::string& content);
    std::string extractTemplateContent(const std::string& content);
    std::map<std::string, std::string> parseStyleProperties(const std::string& content);
    std::vector<std::shared_ptr<CHTLNode>> parseElementChildren(const std::string& content);
    std::map<std::string, std::string> parseVariableProperties(const std::string& content);
    
    // 应用辅助函数
    std::string replaceStyleTemplate(const std::string& content, const TemplateInfo& template_info);
    std::string replaceElementTemplate(const std::string& content, const TemplateInfo& template_info);
    std::string replaceVariableTemplate(const std::string& content, const TemplateInfo& template_info);
    
    // 工具函数
    std::string trim(const std::string& str) const;
    std::vector<std::string> split(const std::string& str, char delimiter) const;
    bool startsWith(const std::string& str, const std::string& prefix) const;
    bool endsWith(const std::string& str, const std::string& suffix) const;
};

} // namespace CHTL