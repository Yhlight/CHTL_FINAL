#pragma once

#include "CHTL/Common.h"
#include "CHTL/BaseNode.h"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>

namespace CHTL {

// Forward declarations
class TemplateNode;
class CustomNode;
class ImportNode;
class NamespaceNode;
class ConfigurationNode;

/**
 * CHTL上下文 - 管理编译状态和符号
 */
class CHTLContext {
public:
    // 符号信息
    struct SymbolInfo {
        std::string name;
        std::string type;
        std::shared_ptr<BaseNode> node;
        std::string namespace_;
        bool defined;
        
        SymbolInfo() : defined(false) {}
    };

public:
    CHTLContext();
    ~CHTLContext();

    // 调试模式
    void setDebugMode(bool enabled);
    bool isDebugMode() const;

    // 命名空间管理
    void setCurrentNamespace(const std::string& namespaceName);
    std::string getCurrentNamespace() const;
    void pushNamespace(const std::string& namespaceName);
    void popNamespace();

    // 符号管理
    void addSymbol(const std::string& name, const std::string& type, std::shared_ptr<BaseNode> node);
    void removeSymbol(const std::string& name);
    std::shared_ptr<BaseNode> getSymbol(const std::string& name) const;
    std::shared_ptr<BaseNode> getSymbol(const std::string& name, const std::string& namespaceName) const;
    bool hasSymbol(const std::string& name) const;
    bool hasSymbol(const std::string& name, const std::string& namespaceName) const;
    std::vector<std::string> getSymbolNames() const;
    std::vector<std::string> getSymbolNames(const std::string& type) const;
    std::vector<std::string> getSymbolNames(const std::string& type, const std::string& namespaceName) const;

    // 模板管理
    void addTemplate(const std::string& name, std::shared_ptr<TemplateNode> templateNode);
    std::shared_ptr<TemplateNode> getTemplate(const std::string& name) const;

    // 自定义管理
    void addCustom(const std::string& name, std::shared_ptr<CustomNode> customNode);
    std::shared_ptr<CustomNode> getCustom(const std::string& name) const;

    // 导入管理
    void addImport(const std::string& name, std::shared_ptr<ImportNode> importNode);
    std::shared_ptr<ImportNode> getImport(const std::string& name) const;

    // 命名空间管理
    void addNamespace(const std::string& name, std::shared_ptr<NamespaceNode> namespaceNode);
    std::shared_ptr<NamespaceNode> getNamespace(const std::string& name) const;

    // 配置管理
    void addConfiguration(const std::string& name, std::shared_ptr<ConfigurationNode> configNode);
    std::shared_ptr<ConfigurationNode> getConfigurationNode(const std::string& name) const;
    void setConfiguration(const std::string& key, const std::string& value);
    std::string getConfiguration(const std::string& key) const;
    bool hasConfiguration(const std::string& key) const;
    void setConfiguration(const std::map<std::string, std::string>& config);
    std::map<std::string, std::string> getConfiguration() const;

    // 变量管理
    void addVariable(const std::string& name, const std::string& value);
    std::string getVariable(const std::string& name) const;
    bool hasVariable(const std::string& name) const;
    void removeVariable(const std::string& name);
    void setVariables(const std::map<std::string, std::string>& vars);
    std::map<std::string, std::string> getVariables() const;

    // 验证
    bool isValidElement(const std::string& elementName) const;
    bool isValidAttribute(const std::string& attributeName) const;
    bool isValidCSSProperty(const std::string& propertyName) const;

    // 自定义验证
    void addCustomElement(const std::string& elementName);
    void addCustomAttribute(const std::string& attributeName);
    void addCustomCSSProperty(const std::string& propertyName);
    bool isCustomElement(const std::string& elementName) const;
    bool isCustomAttribute(const std::string& attributeName) const;
    bool isCustomCSSProperty(const std::string& propertyName) const;

    // 错误和警告
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    std::vector<std::string> getErrors() const;
    std::vector<std::string> getWarnings() const;
    void clearErrors();
    void clearWarnings();

    // 清理
    void clearAll();
    void reset();

    // 信息输出
    std::string getSymbolInfo() const;
    void printSymbols() const;
    void printConfiguration() const;
    void printVariables() const;

private:
    void initializeContext();
    void initializeBuiltinSymbols();
    void initializeDefaultConfiguration();

private:
    std::string currentNamespace_;
    bool debugMode_;
    
    // 符号表
    std::map<std::string, SymbolInfo> symbols_;
    
    // 配置
    std::map<std::string, std::string> configuration_;
    
    // 变量
    std::map<std::string, std::string> variables_;
    
    // 命名空间栈
    std::vector<std::string> namespaceStack_;
    
    // 内置符号
    std::set<std::string> builtinElements_;
    std::set<std::string> builtinAttributes_;
    std::set<std::string> builtinCSSProperties_;
    
    // 自定义符号
    std::set<std::string> customElements_;
    std::set<std::string> customAttributes_;
    std::set<std::string> customCSSProperties_;
    
    // 错误和警告
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
};

} // namespace CHTL