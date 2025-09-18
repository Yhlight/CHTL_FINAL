#ifndef TEMPLATE_SYSTEM_H
#define TEMPLATE_SYSTEM_H

#include "CHTLNode/BaseNode.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>

namespace CHTL {

// 模板类型
enum class TemplateType {
    STYLE,      // @Style
    ELEMENT,    // @Element
    VAR,        // @Var
    UNKNOWN
};

// 自定义类型
enum class CustomType {
    STYLE,      // @Style
    ELEMENT,    // @Element
    VAR,        // @Var
    UNKNOWN
};

// 模板定义
class Template {
public:
    Template(TemplateType type, const std::string& name);
    ~Template() = default;
    
    // 基本信息
    TemplateType getType() const { return type_; }
    const std::string& getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }
    
    // 内容管理
    void setContent(const std::string& content);
    const std::string& getContent() const { return content_; }
    
    // 参数管理
    void addParameter(const std::string& name, const std::string& defaultValue = "");
    std::string getParameter(const std::string& name) const;
    bool hasParameter(const std::string& name) const;
    const std::unordered_map<std::string, std::string>& getParameters() const { return parameters_; }
    
    // 继承管理
    void setParent(const std::string& parentName);
    const std::string& getParent() const { return parentName_; }
    bool hasParent() const { return !parentName_.empty(); }
    
    // 约束管理
    void addConstraint(const std::string& constraint);
    const std::vector<std::string>& getConstraints() const { return constraints_; }
    
    // 渲染
    std::string render(const std::unordered_map<std::string, std::string>& variables = {}) const;
    
private:
    TemplateType type_;
    std::string name_;
    std::string content_;
    std::unordered_map<std::string, std::string> parameters_;
    std::string parentName_;
    std::vector<std::string> constraints_;
    
    // 渲染辅助方法
    std::string processInheritance(const std::string& content) const;
    std::string processParameters(const std::string& content, const std::unordered_map<std::string, std::string>& variables) const;
    std::string processConstraints(const std::string& content) const;
};

// 自定义定义
class Custom {
public:
    Custom(CustomType type, const std::string& name);
    ~Custom() = default;
    
    // 基本信息
    CustomType getType() const { return type_; }
    const std::string& getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }
    
    // 内容管理
    void setContent(const std::string& content);
    const std::string& getContent() const { return content_; }
    
    // 操作管理
    void addOperation(const std::string& operation);
    const std::vector<std::string>& getOperations() const { return operations_; }
    
    // 目标管理
    void setTarget(const std::string& target);
    const std::string& getTarget() const { return target_; }
    
    // 条件管理
    void setCondition(const std::string& condition);
    const std::string& getCondition() const { return condition_; }
    
    // 应用
    std::string apply(const std::string& targetContent) const;
    
private:
    CustomType type_;
    std::string name_;
    std::string content_;
    std::vector<std::string> operations_;
    std::string target_;
    std::string condition_;
    
    // 应用辅助方法
    std::string processOperations(const std::string& targetContent) const;
    std::string processInsert(const std::string& targetContent) const;
    std::string processDelete(const std::string& targetContent) const;
    std::string processReplace(const std::string& targetContent) const;
    std::string processCondition(const std::string& targetContent) const;
};

// 模板系统
class TemplateSystem {
public:
    TemplateSystem();
    ~TemplateSystem() = default;
    
    // 模板管理
    void registerTemplate(std::shared_ptr<Template> template_);
    std::shared_ptr<Template> getTemplate(const std::string& name) const;
    bool hasTemplate(const std::string& name) const;
    void removeTemplate(const std::string& name);
    
    // 自定义管理
    void registerCustom(std::shared_ptr<Custom> custom);
    std::shared_ptr<Custom> getCustom(const std::string& name) const;
    bool hasCustom(const std::string& name) const;
    void removeCustom(const std::string& name);
    
    // 模板渲染
    std::string renderTemplate(const std::string& name, const std::unordered_map<std::string, std::string>& variables = {}) const;
    std::string renderCustom(const std::string& name, const std::string& targetContent) const;
    
    // 模板继承
    std::string resolveInheritance(const std::string& templateName) const;
    
    // 模板验证
    bool validateTemplate(const std::string& name) const;
    bool validateCustom(const std::string& name) const;
    
    // 错误处理
    void setErrorHandler(std::function<void(const std::string&, size_t, size_t)> handler);
    
    // 清理
    void clear();
    
private:
    std::unordered_map<std::string, std::shared_ptr<Template>> templates_;
    std::unordered_map<std::string, std::shared_ptr<Custom>> customs_;
    std::function<void(const std::string&, size_t, size_t)> errorHandler_;
    
    // 辅助方法
    void reportError(const std::string& message, size_t line = 0, size_t column = 0);
    std::string processTemplateContent(const std::string& content, const std::unordered_map<std::string, std::string>& variables) const;
    std::string processCustomContent(const std::string& content, const std::string& targetContent) const;
    
    // 模板解析
    std::string parseTemplateBlock(const std::string& content) const;
    std::string parseCustomBlock(const std::string& content) const;
    
    // 变量替换
    std::string replaceVariables(const std::string& content, const std::unordered_map<std::string, std::string>& variables) const;
    
    // 条件处理
    std::string processConditionals(const std::string& content) const;
    
    // 循环处理
    std::string processLoops(const std::string& content) const;
};

} // namespace CHTL

#endif // TEMPLATE_SYSTEM_H