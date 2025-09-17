#ifndef CHTL_CONTEXT_H
#define CHTL_CONTEXT_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <set>

namespace CHTL {

/**
 * CHTL上下文管理
 * 管理CHTL编译时的全局状态、变量、模板、命名空间等
 */
class CHTLContext {
public:
    // 变量类型
    enum class VariableType {
        TEMPLATE_VAR,       // 模板变量
        CUSTOM_VAR,         // 自定义变量
        RESPONSIVE_VAR,     // 响应式变量
        NAMESPACE_VAR,      // 命名空间变量
        LOCAL_VAR          // 局部变量
    };
    
    // 模板类型
    enum class TemplateType {
        TEMPLATE_STYLE,     // 模板样式组
        TEMPLATE_ELEMENT,   // 模板元素
        TEMPLATE_VAR,       // 模板变量组
        CUSTOM_STYLE,       // 自定义样式组
        CUSTOM_ELEMENT,     // 自定义元素
        CUSTOM_VAR          // 自定义变量组
    };
    
    // 变量信息
    struct VariableInfo {
        std::string name;
        std::string value;
        VariableType type;
        int line;
        int column;
        std::string scope;  // 作用域
        bool isSpecialized; // 是否特例化
        std::string parentName; // 父变量名（用于特例化）
        
        VariableInfo(const std::string& n, const std::string& v, VariableType t, 
                    int l = 0, int c = 0, const std::string& s = "")
            : name(n), value(v), type(t), line(l), column(c), scope(s), 
              isSpecialized(false) {}
    };
    
    // 模板信息
    struct TemplateInfo {
        std::string name;
        TemplateType type;
        std::map<std::string, std::string> properties;
        std::vector<std::shared_ptr<CHTLBaseNode>> children;
        int line;
        int column;
        std::string scope;
        bool isSpecialized;
        std::string parentName;
        
        TemplateInfo(const std::string& n, TemplateType t, int l = 0, int c = 0, 
                    const std::string& s = "")
            : name(n), type(t), line(l), column(c), scope(s), isSpecialized(false) {}
    };
    
    // 命名空间信息
    struct NamespaceInfo {
        std::string name;
        std::string scope;
        std::map<std::string, std::string> imports;
        std::vector<std::shared_ptr<CHTLBaseNode>> children;
        int line;
        int column;
        
        NamespaceInfo(const std::string& n, const std::string& s = "")
            : name(n), scope(s), line(0), column(0) {}
    };
    
    // 配置信息
    struct ConfigurationInfo {
        std::string name;
        std::map<std::string, std::string> settings;
        int line;
        int column;
        std::string scope;
        
        ConfigurationInfo(const std::string& n, const std::string& s = "")
            : name(n), scope(s), line(0), column(0) {}
    };
    
    CHTLContext();
    ~CHTLContext();
    
    // 变量管理
    void setVariable(const std::string& name, const std::string& value, 
                    VariableType type, int line = 0, int column = 0, 
                    const std::string& scope = "");
    VariableInfo* getVariable(const std::string& name, const std::string& scope = "");
    bool hasVariable(const std::string& name, const std::string& scope = "") const;
    void removeVariable(const std::string& name, const std::string& scope = "");
    std::vector<VariableInfo> getVariablesByType(VariableType type) const;
    std::vector<VariableInfo> getVariablesByScope(const std::string& scope) const;
    
    // 变量特例化
    void specializeVariable(const std::string& parentName, const std::string& childName, 
                           const std::string& value, int line = 0, int column = 0);
    std::vector<VariableInfo> getSpecializedVariables(const std::string& parentName) const;
    
    // 模板管理
    void setTemplate(const std::string& name, TemplateType type, 
                   const std::map<std::string, std::string>& properties = {},
                   const std::vector<std::shared_ptr<CHTLBaseNode>>& children = {},
                   int line = 0, int column = 0, const std::string& scope = "");
    TemplateInfo* getTemplate(const std::string& name, const std::string& scope = "");
    bool hasTemplate(const std::string& name, const std::string& scope = "") const;
    void removeTemplate(const std::string& name, const std::string& scope = "");
    std::vector<TemplateInfo> getTemplatesByType(TemplateType type) const;
    std::vector<TemplateInfo> getTemplatesByScope(const std::string& scope) const;
    
    // 模板特例化
    void specializeTemplate(const std::string& parentName, const std::string& childName,
                           const std::map<std::string, std::string>& properties = {},
                           const std::vector<std::shared_ptr<CHTLBaseNode>>& children = {},
                           int line = 0, int column = 0);
    std::vector<TemplateInfo> getSpecializedTemplates(const std::string& parentName) const;
    
    // 命名空间管理
    void setNamespace(const std::string& name, const std::string& scope = "");
    NamespaceInfo* getNamespace(const std::string& name, const std::string& scope = "");
    bool hasNamespace(const std::string& name, const std::string& scope = "") const;
    void removeNamespace(const std::string& name, const std::string& scope = "");
    void addNamespaceImport(const std::string& namespaceName, const std::string& importName, 
                           const std::string& value, const std::string& scope = "");
    std::vector<NamespaceInfo> getAllNamespaces() const;
    
    // 配置管理
    void setConfiguration(const std::string& name, const std::map<std::string, std::string>& settings,
                         int line = 0, int column = 0, const std::string& scope = "");
    ConfigurationInfo* getConfiguration(const std::string& name, const std::string& scope = "");
    bool hasConfiguration(const std::string& name, const std::string& scope = "") const;
    void removeConfiguration(const std::string& name, const std::string& scope = "");
    std::string getConfigurationValue(const std::string& configName, const std::string& key, 
                                     const std::string& scope = "") const;
    
    // 作用域管理
    void enterScope(const std::string& scopeName);
    void exitScope();
    std::string getCurrentScope() const;
    std::vector<std::string> getScopeHierarchy() const;
    
    // 引用属性管理
    void addPropertyReference(const std::string& selector, const std::string& property, 
                             const std::string& value);
    std::string getPropertyReference(const std::string& selector, const std::string& property) const;
    bool hasPropertyReference(const std::string& selector, const std::string& property) const;
    
    // 响应式值管理
    void setResponsiveValue(const std::string& jsVarName, const std::string& value);
    std::string getResponsiveValue(const std::string& jsVarName) const;
    bool hasResponsiveValue(const std::string& jsVarName) const;
    void removeResponsiveValue(const std::string& jsVarName);
    
    // 局部样式块管理
    void addLocalStyle(const std::string& elementPath, const std::string& selector, 
                      const std::map<std::string, std::string>& properties);
    std::map<std::string, std::string> getLocalStyle(const std::string& elementPath, 
                                                    const std::string& selector) const;
    bool hasLocalStyle(const std::string& elementPath, const std::string& selector) const;
    
    // 编译选项
    void setCompileOption(const std::string& option, bool value);
    bool getCompileOption(const std::string& option) const;
    void setCompileOption(const std::string& option, const std::string& value);
    std::string getCompileOptionString(const std::string& option) const;
    
    // 错误处理
    void setErrorHandler(std::function<void(const std::string&, int, int)> handler);
    void reportError(const std::string& message, int line = 0, int column = 0);
    bool hasErrors() const;
    const std::vector<std::string>& getErrors() const;
    void clearErrors();
    
    // 调试和统计
    void printContext() const;
    size_t getVariableCount() const;
    size_t getTemplateCount() const;
    size_t getNamespaceCount() const;
    size_t getConfigurationCount() const;
    
    // 清理
    void clear();
    void clearScope(const std::string& scope);

private:
    // 变量存储
    std::map<std::string, std::vector<VariableInfo>> variables_; // scope -> variables
    std::map<std::string, std::vector<TemplateInfo>> templates_; // scope -> templates
    std::map<std::string, NamespaceInfo> namespaces_;
    std::map<std::string, ConfigurationInfo> configurations_;
    
    // 引用属性存储
    std::map<std::string, std::map<std::string, std::string>> property_references_; // selector -> property -> value
    
    // 响应式值存储
    std::map<std::string, std::string> responsive_values_; // jsVarName -> value
    
    // 局部样式存储
    std::map<std::string, std::map<std::string, std::map<std::string, std::string>>> local_styles_; // elementPath -> selector -> property -> value
    
    // 作用域管理
    std::vector<std::string> scope_stack_;
    std::string current_scope_;
    
    // 编译选项
    std::map<std::string, bool> bool_options_;
    std::map<std::string, std::string> string_options_;
    
    // 错误处理
    std::function<void(const std::string&, int, int)> error_handler_;
    std::vector<std::string> errors_;
    
    // 辅助方法
    std::string getFullVariableName(const std::string& name, const std::string& scope) const;
    std::string getFullTemplateName(const std::string& name, const std::string& scope) const;
    void logError(const std::string& message, int line = 0, int column = 0);
    bool isValidScope(const std::string& scope) const;
    std::string resolveScope(const std::string& scope) const;
};

} // namespace CHTL

#endif // CHTL_CONTEXT_H