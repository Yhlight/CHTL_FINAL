#pragma once

#include "CHTLNode.h"
#include "CHTLContext.h"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <json/json.h>

namespace CHTL {

/**
 * @brief 语法高亮规则
 */
class SyntaxHighlightRule {
public:
    SyntaxHighlightRule(const std::string& pattern, const std::string& tokenType, 
                       const std::string& foreground = "", const std::string& background = "");
    ~SyntaxHighlightRule() = default;

    std::string getPattern() const { return m_pattern; }
    std::string getTokenType() const { return m_tokenType; }
    std::string getForeground() const { return m_foreground; }
    std::string getBackground() const { return m_background; }
    
    void setForeground(const std::string& color) { m_foreground = color; }
    void setBackground(const std::string& color) { m_background = color; }

private:
    std::string m_pattern;
    std::string m_tokenType;
    std::string m_foreground;
    std::string m_background;
};

/**
 * @brief 语法高亮器
 */
class SyntaxHighlighter {
public:
    explicit SyntaxHighlighter(std::shared_ptr<CHTLContext> context);
    ~SyntaxHighlighter() = default;

    // 语法高亮
    std::vector<std::pair<int, std::string>> highlight(const std::string& code);
    std::string generateHighlightedHTML(const std::string& code);
    std::string generateHighlightedCSS(const std::string& code);
    
    // 规则管理
    void addRule(const SyntaxHighlightRule& rule);
    void addRule(const std::string& pattern, const std::string& tokenType, 
                const std::string& foreground = "", const std::string& background = "");
    void removeRule(const std::string& pattern);
    void clearRules();
    
    // 主题管理
    void setTheme(const std::string& themeName);
    void loadTheme(const std::string& themeFile);
    void saveTheme(const std::string& themeFile);

private:
    std::shared_ptr<CHTLContext> m_context;
    std::vector<SyntaxHighlightRule> m_rules;
    std::string m_currentTheme;
    
    // 辅助方法
    void initializeDefaultRules();
    std::string getTokenColor(const std::string& tokenType) const;
    std::string escapeHTML(const std::string& text) const;
};

/**
 * @brief 代码格式化器
 */
class CodeFormatter {
public:
    explicit CodeFormatter(std::shared_ptr<CHTLContext> context);
    ~CodeFormatter() = default;

    // 代码格式化
    std::string format(const std::string& code);
    std::string formatFile(const std::string& filePath);
    void formatFileInPlace(const std::string& filePath);
    
    // 格式化选项
    void setIndentSize(int size) { m_indentSize = size; }
    void setUseTabs(bool useTabs) { m_useTabs = useTabs; }
    void setMaxLineLength(int length) { m_maxLineLength = length; }
    void setInsertFinalNewline(bool insert) { m_insertFinalNewline = insert; }
    
    // 格式化规则
    void addFormattingRule(const std::string& pattern, const std::string& replacement);
    void removeFormattingRule(const std::string& pattern);
    void clearFormattingRules();

private:
    std::shared_ptr<CHTLContext> m_context;
    int m_indentSize;
    bool m_useTabs;
    int m_maxLineLength;
    bool m_insertFinalNewline;
    std::unordered_map<std::string, std::string> m_formattingRules;
    
    // 辅助方法
    void initializeDefaultRules();
    std::string indentLine(const std::string& line, int level) const;
    std::string getIndentString() const;
    bool shouldBreakLine(const std::string& line) const;
};

/**
 * @brief 代码提示器
 */
class CodeCompleter {
public:
    explicit CodeCompleter(std::shared_ptr<CHTLContext> context);
    ~CodeCompleter() = default;

    // 代码补全
    std::vector<std::string> getCompletions(const std::string& code, int position);
    std::vector<std::string> getCompletionsForContext(const std::string& code, int position);
    
    // 提示类型
    std::vector<std::string> getElementCompletions();
    std::vector<std::string> getAttributeCompletions();
    std::vector<std::string> getTemplateCompletions();
    std::vector<std::string> getCustomCompletions();
    std::vector<std::string> getImportCompletions();
    std::vector<std::string> getConfigCompletions();
    
    // 智能提示
    std::vector<std::string> getSmartCompletions(const std::string& code, int position);
    std::vector<std::string> getContextualCompletions(const std::string& code, int position);
    std::vector<std::string> getSnippetCompletions(const std::string& code, int position);
    
    // 提示管理
    void addCompletion(const std::string& trigger, const std::string& completion);
    void removeCompletion(const std::string& trigger);
    void clearCompletions();

private:
    std::shared_ptr<CHTLContext> m_context;
    std::unordered_map<std::string, std::string> m_completions;
    std::vector<std::string> m_elementCompletions;
    std::vector<std::string> m_attributeCompletions;
    std::vector<std::string> m_templateCompletions;
    std::vector<std::string> m_customCompletions;
    std::vector<std::string> m_importCompletions;
    std::vector<std::string> m_configCompletions;
    
    // 辅助方法
    void initializeDefaultCompletions();
    std::string getContextAtPosition(const std::string& code, int position) const;
    bool isInElementContext(const std::string& code, int position) const;
    bool isInAttributeContext(const std::string& code, int position) const;
    bool isInTemplateContext(const std::string& code, int position) const;
    bool isInCustomContext(const std::string& code, int position) const;
    bool isInImportContext(const std::string& code, int position) const;
    bool isInConfigContext(const std::string& code, int position) const;
};

/**
 * @brief 页面预览器
 */
class PagePreviewer {
public:
    explicit PagePreviewer(std::shared_ptr<CHTLContext> context);
    ~PagePreviewer() = default;

    // 页面预览
    std::string generatePreview(const std::string& chtlCode);
    std::string generatePreviewFromFile(const std::string& filePath);
    void startLivePreview(const std::string& filePath);
    void stopLivePreview();
    bool isLivePreviewActive() const;
    
    // 预览选项
    void setPreviewMode(const std::string& mode) { m_previewMode = mode; }
    void setAutoRefresh(bool enabled) { m_autoRefresh = enabled; }
    void setRefreshInterval(int milliseconds) { m_refreshInterval = milliseconds; }
    
    // 预览管理
    void addPreviewListener(std::function<void(const std::string&)> listener);
    void removePreviewListener(std::function<void(const std::string&)> listener);
    void clearPreviewListeners();

private:
    std::shared_ptr<CHTLContext> m_context;
    std::string m_previewMode;
    bool m_autoRefresh;
    int m_refreshInterval;
    bool m_isLivePreviewActive;
    std::vector<std::function<void(const std::string&)>> m_previewListeners;
    
    // 辅助方法
    std::string generateHTMLPreview(const std::string& chtlCode) const;
    std::string generateCSSPreview(const std::string& chtlCode) const;
    std::string generateJavaScriptPreview(const std::string& chtlCode) const;
    void notifyPreviewListeners(const std::string& preview) const;
};

/**
 * @brief 导出器
 */
class Exporter {
public:
    explicit Exporter(std::shared_ptr<CHTLContext> context);
    ~Exporter() = default;

    // 导出功能
    void exportToHTML(const std::string& chtlCode, const std::string& outputPath);
    void exportToCSS(const std::string& chtlCode, const std::string& outputPath);
    void exportToJavaScript(const std::string& chtlCode, const std::string& outputPath);
    void exportToFiles(const std::string& chtlCode, const std::string& outputDirectory);
    
    // 导出选项
    void setExportFormat(const std::string& format) { m_exportFormat = format; }
    void setMinifyOutput(bool minify) { m_minifyOutput = minify; }
    void setIncludeSourceMaps(bool include) { m_includeSourceMaps = include; }
    
    // 批量导出
    void exportDirectory(const std::string& inputDirectory, const std::string& outputDirectory);
    void exportProject(const std::string& projectPath, const std::string& outputPath);

private:
    std::shared_ptr<CHTLContext> m_context;
    std::string m_exportFormat;
    bool m_minifyOutput;
    bool m_includeSourceMaps;
    
    // 辅助方法
    std::string minifyHTML(const std::string& html) const;
    std::string minifyCSS(const std::string& css) const;
    std::string minifyJavaScript(const std::string& javascript) const;
    std::string generateSourceMap(const std::string& source, const std::string& output) const;
};

/**
 * @brief 内置编译器
 */
class BuiltinCompiler {
public:
    explicit BuiltinCompiler(std::shared_ptr<CHTLContext> context);
    ~BuiltinCompiler() = default;

    // 编译功能
    std::string compile(const std::string& chtlCode);
    std::string compileFile(const std::string& filePath);
    void compileToFile(const std::string& chtlCode, const std::string& outputPath);
    void compileFileToFile(const std::string& inputPath, const std::string& outputPath);
    
    // 编译选项
    void setCompilationMode(const std::string& mode) { m_compilationMode = mode; }
    void setOptimizationLevel(int level) { m_optimizationLevel = level; }
    void setDebugMode(bool enabled) { m_debugMode = enabled; }
    
    // 编译状态
    bool isCompiling() const { return m_isCompiling; }
    std::vector<std::string> getCompilationErrors() const { return m_compilationErrors; }
    std::vector<std::string> getCompilationWarnings() const { return m_compilationWarnings; }

private:
    std::shared_ptr<CHTLContext> m_context;
    std::string m_compilationMode;
    int m_optimizationLevel;
    bool m_debugMode;
    bool m_isCompiling;
    std::vector<std::string> m_compilationErrors;
    std::vector<std::string> m_compilationWarnings;
    
    // 辅助方法
    void clearCompilationState();
    void addCompilationError(const std::string& error);
    void addCompilationWarning(const std::string& warning);
    std::string optimizeCode(const std::string& code) const;
};

/**
 * @brief 模块解包器
 */
class ModuleUnpacker {
public:
    explicit ModuleUnpacker(std::shared_ptr<CHTLContext> context);
    ~ModuleUnpacker() = default;

    // 模块解包
    void unpackModule(const std::string& modulePath, const std::string& outputPath);
    void unpackAllModules(const std::string& projectPath);
    void unpackModuleDependencies(const std::string& modulePath);
    
    // 模块查询
    std::vector<std::string> findModules(const std::string& projectPath);
    std::vector<std::string> findModuleDependencies(const std::string& modulePath);
    std::string getModuleInfo(const std::string& modulePath);
    
    // 模块管理
    void installModule(const std::string& modulePath);
    void uninstallModule(const std::string& moduleName);
    void updateModule(const std::string& moduleName);

private:
    std::shared_ptr<CHTLContext> m_context;
    
    // 辅助方法
    bool isModuleFile(const std::string& filePath) const;
    std::string extractModuleName(const std::string& modulePath) const;
    std::string getModuleVersion(const std::string& modulePath) const;
    std::vector<std::string> parseModuleDependencies(const std::string& modulePath) const;
};

/**
 * @brief JSON 查询表
 */
class JSONQueryTable {
public:
    explicit JSONQueryTable(std::shared_ptr<CHTLContext> context);
    ~JSONQueryTable() = default;

    // 查询功能
    std::string query(const std::string& queryString);
    std::string queryFile(const std::string& filePath, const std::string& queryString);
    std::vector<std::string> queryMultiple(const std::string& queryString);
    
    // 数据管理
    void addData(const std::string& key, const Json::Value& value);
    void addDataFromFile(const std::string& key, const std::string& filePath);
    void removeData(const std::string& key);
    void clearData();
    
    // 查询优化
    void createIndex(const std::string& field);
    void removeIndex(const std::string& field);
    void optimizeQueries();

private:
    std::shared_ptr<CHTLContext> m_context;
    std::unordered_map<std::string, Json::Value> m_data;
    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>> m_indexes;
    
    // 辅助方法
    Json::Value parseQuery(const std::string& queryString) const;
    Json::Value executeQuery(const Json::Value& query) const;
    std::vector<std::string> searchIndex(const std::string& field, const std::string& value) const;
};

/**
 * @brief 模块语法提示器
 */
class ModuleSyntaxHelper {
public:
    explicit ModuleSyntaxHelper(std::shared_ptr<CHTLContext> context);
    ~ModuleSyntaxHelper() = default;

    // 语法提示
    std::vector<std::string> getModuleSyntaxHints(const std::string& moduleName);
    std::vector<std::string> getTemplateSyntaxHints(const std::string& templateName);
    std::vector<std::string> getCustomSyntaxHints(const std::string& customName);
    std::vector<std::string> getImportSyntaxHints(const std::string& importPath);
    
    // 语法验证
    bool validateModuleSyntax(const std::string& moduleCode);
    bool validateTemplateSyntax(const std::string& templateCode);
    bool validateCustomSyntax(const std::string& customCode);
    bool validateImportSyntax(const std::string& importCode);
    
    // 语法帮助
    std::string getSyntaxHelp(const std::string& syntaxType);
    std::string getModuleHelp(const std::string& moduleName);
    std::string getTemplateHelp(const std::string& templateName);
    std::string getCustomHelp(const std::string& customName);

private:
    std::shared_ptr<CHTLContext> m_context;
    
    // 辅助方法
    void initializeSyntaxHints();
    std::vector<std::string> parseModuleSyntax(const std::string& moduleCode) const;
    std::vector<std::string> parseTemplateSyntax(const std::string& templateCode) const;
    std::vector<std::string> parseCustomSyntax(const std::string& customCode) const;
    std::vector<std::string> parseImportSyntax(const std::string& importCode) const;
};

/**
 * @brief VSCode 扩展管理器
 */
class VSCodeExtensionManager {
public:
    explicit VSCodeExtensionManager(std::shared_ptr<CHTLContext> context);
    ~VSCodeExtensionManager() = default;

    // 扩展管理
    void initialize();
    void shutdown();
    void reload();
    
    // 组件获取
    std::shared_ptr<SyntaxHighlighter> getSyntaxHighlighter() const;
    std::shared_ptr<CodeFormatter> getCodeFormatter() const;
    std::shared_ptr<CodeCompleter> getCodeCompleter() const;
    std::shared_ptr<PagePreviewer> getPagePreviewer() const;
    std::shared_ptr<Exporter> getExporter() const;
    std::shared_ptr<BuiltinCompiler> getBuiltinCompiler() const;
    std::shared_ptr<ModuleUnpacker> getModuleUnpacker() const;
    std::shared_ptr<JSONQueryTable> getJSONQueryTable() const;
    std::shared_ptr<ModuleSyntaxHelper> getModuleSyntaxHelper() const;
    
    // 扩展配置
    void loadConfiguration(const std::string& configFile);
    void saveConfiguration(const std::string& configFile);
    void setConfiguration(const std::string& key, const std::string& value);
    std::string getConfiguration(const std::string& key) const;

private:
    std::shared_ptr<CHTLContext> m_context;
    std::shared_ptr<SyntaxHighlighter> m_syntaxHighlighter;
    std::shared_ptr<CodeFormatter> m_codeFormatter;
    std::shared_ptr<CodeCompleter> m_codeCompleter;
    std::shared_ptr<PagePreviewer> m_pagePreviewer;
    std::shared_ptr<Exporter> m_exporter;
    std::shared_ptr<BuiltinCompiler> m_builtinCompiler;
    std::shared_ptr<ModuleUnpacker> m_moduleUnpacker;
    std::shared_ptr<JSONQueryTable> m_jsonQueryTable;
    std::shared_ptr<ModuleSyntaxHelper> m_moduleSyntaxHelper;
    std::unordered_map<std::string, std::string> m_configuration;
    
    // 辅助方法
    void initializeComponents();
    void loadDefaultConfiguration();
    void saveDefaultConfiguration();
};

} // namespace CHTL