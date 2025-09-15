#ifndef CHTL_CONTEXT_H
#define CHTL_CONTEXT_H

#include "../CHTLParser/CHTLASTNode.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>

namespace CHTL {

enum class ContextType {
    GLOBAL,         // 全局上下文
    TEMPLATE,       // 模板上下文
    CUSTOM,         // 自定义上下文
    NAMESPACE,      // 命名空间上下文
    LOCAL           // 局部上下文
};

struct TemplateInfo {
    std::string name;
    std::string type; // @Style, @Element, @Var
    std::shared_ptr<CHTLASTNode> content;
    std::vector<std::string> parameters;
    std::unordered_map<std::string, std::string> defaults;
    bool isPublic;
    
    TemplateInfo() : isPublic(false) {}
};

struct CustomInfo {
    std::string name;
    std::string type; // @Style, @Element, @Var
    std::shared_ptr<CHTLASTNode> content;
    std::vector<std::string> parameters;
    std::unordered_map<std::string, std::string> defaults;
    bool isPublic;
    
    CustomInfo() : isPublic(false) {}
};

struct VariableInfo {
    std::string name;
    std::string value;
    std::string type; // string, number, boolean, object
    bool isConstant;
    bool isPublic;
    
    VariableInfo() : isConstant(false), isPublic(false) {}
};

struct NamespaceInfo {
    std::string name;
    std::string prefix;
    std::unordered_map<std::string, std::string> imports;
    std::vector<std::string> exports;
    bool isPublic;
    
    NamespaceInfo() : isPublic(false) {}
};

struct ImportInfo {
    std::string name;
    std::string path;
    std::string type; // @Html, @Style, @JavaScript, @Chtl, @CJmod
    std::string alias;
    std::string from;
    bool isResolved;
    
    ImportInfo() : isResolved(false) {}
};

struct ConfigurationInfo {
    std::string name;
    std::unordered_map<std::string, std::string> settings;
    bool isPublic;
    
    ConfigurationInfo() : isPublic(false) {}
};

class CHTLContext {
private:
    ContextType type;
    std::string name;
    std::shared_ptr<CHTLContext> parent;
    std::vector<std::shared_ptr<CHTLContext>> children;
    
    // 模板管理
    std::unordered_map<std::string, TemplateInfo> templates;
    std::unordered_map<std::string, CustomInfo> customElements;
    
    // 变量管理
    std::unordered_map<std::string, VariableInfo> variables;
    std::unordered_map<std::string, std::string> constants;
    
    // 命名空间管理
    std::unordered_map<std::string, NamespaceInfo> namespaces;
    std::unordered_map<std::string, std::string> namespacePrefixes;
    
    // 导入管理
    std::vector<ImportInfo> imports;
    std::unordered_map<std::string, ImportInfo> importMap;
    
    // 配置管理
    std::unordered_map<std::string, ConfigurationInfo> configurations;
    
    // 内部方法
    std::shared_ptr<CHTLContext> findContext(const std::string& name) const;
    std::shared_ptr<CHTLContext> findContextByType(ContextType type) const;
    bool validateName(const std::string& name) const;
    std::string generateUniqueName(const std::string& base) const;
    
public:
    CHTLContext(ContextType contextType = ContextType::GLOBAL, const std::string& contextName = "");
    ~CHTLContext() = default;
    
    // 上下文管理
    void setParent(std::shared_ptr<CHTLContext> parentContext);
    std::shared_ptr<CHTLContext> getParent() const;
    void addChild(std::shared_ptr<CHTLContext> childContext);
    std::vector<std::shared_ptr<CHTLContext>> getChildren() const;
    void removeChild(const std::string& childName);
    
    // 基本信息
    ContextType getType() const;
    std::string getName() const;
    void setName(const std::string& name);
    
    // 模板管理
    void addTemplate(const TemplateInfo& templateInfo);
    void removeTemplate(const std::string& name);
    bool hasTemplate(const std::string& name) const;
    TemplateInfo getTemplate(const std::string& name) const;
    std::vector<TemplateInfo> getAllTemplates() const;
    std::vector<TemplateInfo> getTemplatesByType(const std::string& type) const;
    
    // 自定义元素管理
    void addCustomElement(const CustomInfo& customInfo);
    void removeCustomElement(const std::string& name);
    bool hasCustomElement(const std::string& name) const;
    CustomInfo getCustomElement(const std::string& name) const;
    std::vector<CustomInfo> getAllCustomElements() const;
    std::vector<CustomInfo> getCustomElementsByType(const std::string& type) const;
    
    // 变量管理
    void setVariable(const std::string& name, const std::string& value, 
                    const std::string& type = "string", bool isConstant = false);
    void removeVariable(const std::string& name);
    bool hasVariable(const std::string& name) const;
    VariableInfo getVariable(const std::string& name) const;
    std::string getVariableValue(const std::string& name) const;
    std::vector<VariableInfo> getAllVariables() const;
    
    // 常量管理
    void setConstant(const std::string& name, const std::string& value);
    void removeConstant(const std::string& name);
    bool hasConstant(const std::string& name) const;
    std::string getConstant(const std::string& name) const;
    std::unordered_map<std::string, std::string> getAllConstants() const;
    
    // 命名空间管理
    void addNamespace(const NamespaceInfo& namespaceInfo);
    void removeNamespace(const std::string& name);
    bool hasNamespace(const std::string& name) const;
    NamespaceInfo getNamespace(const std::string& name) const;
    std::string getNamespacePrefix(const std::string& name) const;
    std::vector<NamespaceInfo> getAllNamespaces() const;
    
    // 导入管理
    void addImport(const ImportInfo& importInfo);
    void removeImport(const std::string& name);
    bool hasImport(const std::string& name) const;
    ImportInfo getImport(const std::string& name) const;
    std::vector<ImportInfo> getAllImports() const;
    std::vector<ImportInfo> getImportsByType(const std::string& type) const;
    
    // 配置管理
    void addConfiguration(const ConfigurationInfo& configInfo);
    void removeConfiguration(const std::string& name);
    bool hasConfiguration(const std::string& name) const;
    ConfigurationInfo getConfiguration(const std::string& name) const;
    std::vector<ConfigurationInfo> getAllConfigurations() const;
    
    // 查找方法
    std::string resolveName(const std::string& name) const;
    std::string resolveTemplate(const std::string& name) const;
    std::string resolveCustomElement(const std::string& name) const;
    std::string resolveVariable(const std::string& name) const;
    std::string resolveNamespace(const std::string& name) const;
    std::string resolveImport(const std::string& name) const;
    
    // 作用域查找
    std::string findInScope(const std::string& name) const;
    std::string findInParentScope(const std::string& name) const;
    std::string findInGlobalScope(const std::string& name) const;
    
    // 验证方法
    bool validateTemplate(const TemplateInfo& templateInfo) const;
    bool validateCustomElement(const CustomInfo& customInfo) const;
    bool validateVariable(const VariableInfo& variableInfo) const;
    bool validateNamespace(const NamespaceInfo& namespaceInfo) const;
    bool validateImport(const ImportInfo& importInfo) const;
    bool validateConfiguration(const ConfigurationInfo& configInfo) const;
    
    // 清理方法
    void clear();
    void clearTemplates();
    void clearCustomElements();
    void clearVariables();
    void clearConstants();
    void clearNamespaces();
    void clearImports();
    void clearConfigurations();
    
    // 调试方法
    void printContext() const;
    void printTemplates() const;
    void printCustomElements() const;
    void printVariables() const;
    void printConstants() const;
    void printNamespaces() const;
    void printImports() const;
    void printConfigurations() const;
    void printStatistics() const;
};

} // namespace CHTL

#endif // CHTL_CONTEXT_H