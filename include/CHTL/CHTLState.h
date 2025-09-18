#pragma once

#include "CHTL/Common.h"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>

namespace CHTL {

/**
 * CHTL状态 - 跟踪编译状态和变量
 */
class CHTLState {
public:
    // 编译阶段
    enum class CompilationPhase {
        INITIALIZATION,
        LEXING,
        PARSING,
        TEMPLATE_PROCESSING,
        CUSTOM_PROCESSING,
        IMPORT_PROCESSING,
        NAMESPACE_PROCESSING,
        CONFIGURATION_PROCESSING,
        GENERATION,
        FINALIZATION
    };

    // 模板处理状态
    enum class TemplateProcessingState {
        NONE,
        PROCESSING_STYLE_TEMPLATES,
        PROCESSING_ELEMENT_TEMPLATES,
        PROCESSING_VAR_TEMPLATES
    };

    // 自定义处理状态
    enum class CustomProcessingState {
        NONE,
        PROCESSING_CUSTOM_STYLES,
        PROCESSING_CUSTOM_ELEMENTS,
        PROCESSING_CUSTOM_VARS
    };

    // 导入处理状态
    enum class ImportProcessingState {
        NONE,
        PROCESSING_HTML_IMPORTS,
        PROCESSING_CSS_IMPORTS,
        PROCESSING_JS_IMPORTS,
        PROCESSING_CHTL_IMPORTS,
        PROCESSING_CJMOD_IMPORTS
    };

    // 命名空间处理状态
    enum class NamespaceProcessingState {
        NONE,
        PROCESSING_NAMESPACES,
        PROCESSING_NAMESPACE_IMPORTS
    };

    // 配置处理状态
    enum class ConfigurationProcessingState {
        NONE,
        PROCESSING_CONFIGURATIONS,
        PROCESSING_CONFIGURATION_IMPORTS
    };

public:
    CHTLState();
    ~CHTLState();

    // 编译阶段管理
    void setCurrentPhase(CompilationPhase phase);
    CompilationPhase getCurrentPhase() const;
    std::string getPhaseName(CompilationPhase phase) const;

    // 调试模式
    void setDebugMode(bool enabled);
    bool isDebugMode() const;

    // 命名空间管理
    void setCurrentNamespace(const std::string& namespaceName);
    std::string getCurrentNamespace() const;
    void pushNamespace(const std::string& namespaceName);
    void popNamespace();

    // 作用域管理
    void setCurrentScope(size_t scope);
    size_t getCurrentScope() const;
    void pushScope();
    void popScope();

    // 模板处理状态
    void setTemplateProcessingState(TemplateProcessingState state);
    TemplateProcessingState getTemplateProcessingState() const;
    std::string getTemplateProcessingStateName(TemplateProcessingState state) const;

    // 自定义处理状态
    void setCustomProcessingState(CustomProcessingState state);
    CustomProcessingState getCustomProcessingState() const;
    std::string getCustomProcessingStateName(CustomProcessingState state) const;

    // 导入处理状态
    void setImportProcessingState(ImportProcessingState state);
    ImportProcessingState getImportProcessingState() const;
    std::string getImportProcessingStateName(ImportProcessingState state) const;

    // 命名空间处理状态
    void setNamespaceProcessingState(NamespaceProcessingState state);
    NamespaceProcessingState getNamespaceProcessingState() const;
    std::string getNamespaceProcessingStateName(NamespaceProcessingState state) const;

    // 配置处理状态
    void setConfigurationProcessingState(ConfigurationProcessingState state);
    ConfigurationProcessingState getConfigurationProcessingState() const;
    std::string getConfigurationProcessingStateName(ConfigurationProcessingState state) const;

    // 处理状态跟踪
    void addProcessedTemplate(const std::string& templateName);
    bool isTemplateProcessed(const std::string& templateName) const;
    void addProcessedCustom(const std::string& customName);
    bool isCustomProcessed(const std::string& customName) const;
    void addProcessedImport(const std::string& importName);
    bool isImportProcessed(const std::string& importName) const;
    void addProcessedNamespace(const std::string& namespaceName);
    bool isNamespaceProcessed(const std::string& namespaceName) const;
    void addProcessedConfiguration(const std::string& configName);
    bool isConfigurationProcessed(const std::string& configName) const;

    // 错误和警告
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    std::vector<std::string> getErrors() const;
    std::vector<std::string> getWarnings() const;
    void clearErrors();
    void clearWarnings();

    // 变量管理
    void addVariable(const std::string& name, const std::string& value);
    std::string getVariable(const std::string& name) const;
    bool hasVariable(const std::string& name) const;
    void removeVariable(const std::string& name);
    void setVariables(const std::map<std::string, std::string>& vars);
    std::map<std::string, std::string> getVariables() const;

    // 作用域变量管理
    void addScopeVariable(const std::string& name, const std::string& value);
    std::string getScopeVariable(const std::string& name) const;
    bool hasScopeVariable(const std::string& name) const;
    void removeScopeVariable(const std::string& name);
    void clearScopeVariables();

    // 阶段数据管理
    void addPhaseData(const std::string& key, const std::string& value);
    std::string getPhaseData(const std::string& key) const;
    bool hasPhaseData(const std::string& key) const;
    void removePhaseData(const std::string& key);
    void clearPhaseData();

    // 清理
    void clearAll();
    void reset();

    // 信息输出
    std::string getStateInfo() const;
    void printState() const;
    void printProcessedItems() const;

private:
    void initializeState();

private:
    CompilationPhase currentPhase_;
    std::string currentNamespace_;
    size_t currentScope_;
    bool debugMode_;
    
    // 处理状态
    TemplateProcessingState templateProcessingState_;
    CustomProcessingState customProcessingState_;
    ImportProcessingState importProcessingState_;
    NamespaceProcessingState namespaceProcessingState_;
    ConfigurationProcessingState configurationProcessingState_;
    
    // 处理跟踪
    std::set<std::string> processedTemplates_;
    std::set<std::string> processedCustoms_;
    std::set<std::string> processedImports_;
    std::set<std::string> processedNamespaces_;
    std::set<std::string> processedConfigurations_;
    
    // 变量
    std::map<std::string, std::string> variables_;
    std::map<size_t, std::map<std::string, std::string>> scopeVariables_;
    
    // 阶段数据
    std::map<CompilationPhase, std::map<std::string, std::string>> phaseData_;
    
    // 作用域栈
    std::vector<size_t> scopeStack_;
    
    // 命名空间栈
    std::vector<std::string> namespaceStack_;
    
    // 阶段列表
    std::vector<CompilationPhase> phases_;
    
    // 错误和警告
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
};

} // namespace CHTL