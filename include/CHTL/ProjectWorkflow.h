#pragma once

#include "Scanner/UnifiedScanner.h"
#include "CHTL/CHTLCompiler.h"
#include "CHTL/CHTLJSCompiler.h"
#include "CHTL/ConfigurationSystem.h"
#include "CHTL/UseStatement.h"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>

namespace CHTL {

/**
 * 编译器调度器
 */
class CompilerDispatcher {
public:
    CompilerDispatcher();
    ~CompilerDispatcher();
    
    // 编译器注册
    void registerCompiler(const std::string& name, std::shared_ptr<CHTLCompiler> compiler);
    void registerJSCompiler(const std::string& name, std::shared_ptr<CHTLJSCompiler> compiler);
    void registerCSSCompiler(const std::string& name, std::function<std::string(const std::string&)> compiler);
    void registerJSCompiler(const std::string& name, std::function<std::string(const std::string&)> compiler);
    
    // 编译器查询
    std::shared_ptr<CHTLCompiler> getCompiler(const std::string& name) const;
    std::shared_ptr<CHTLJSCompiler> getJSCompiler(const std::string& name) const;
    std::function<std::string(const std::string&)> getCSSCompiler(const std::string& name) const;
    std::function<std::string(const std::string&)> getJSCompiler(const std::string& name) const;
    
    // 编译调度
    std::string dispatchCompilation(const std::string& content, const std::string& type);
    std::string dispatchFileCompilation(const std::string& filePath, const std::string& type);
    
    // 编译器列表
    std::vector<std::string> getCompilerNames() const;
    std::vector<std::string> getJSCompilerNames() const;
    std::vector<std::string> getCSSCompilerNames() const;
    std::vector<std::string> getJSCompilerNames() const;
    
    // 清理
    void clear();
    void clearCompilers();
    void clearJSCompilers();
    void clearCSSCompilers();
    void clearJSCompilers();
    
private:
    std::map<std::string, std::shared_ptr<CHTLCompiler>> compilers_;
    std::map<std::string, std::shared_ptr<CHTLJSCompiler>> js_compilers_;
    std::map<std::string, std::function<std::string(const std::string&)>> css_compilers_;
    std::map<std::string, std::function<std::string(const std::string&)>> js_compilers_;
    
    // 调度辅助函数
    std::string determineCompilerType(const std::string& content) const;
    std::string determineFileType(const std::string& filePath) const;
};

/**
 * 代码合并器
 */
class CodeMerger {
public:
    CodeMerger();
    ~CodeMerger();
    
    // 代码片段管理
    void addCodeFragment(const std::string& name, const std::string& content, const std::string& type);
    void removeCodeFragment(const std::string& name);
    void clearCodeFragments();
    
    // 代码合并
    std::string mergeCode(const std::string& html, const std::string& css, const std::string& js) const;
    std::string mergeCodeFragments() const;
    std::string mergeCodeFragmentsByType(const std::string& type) const;
    
    // 代码查询
    std::vector<std::string> getCodeFragmentNames() const;
    std::vector<std::string> getCodeFragmentNamesByType(const std::string& type) const;
    std::string getCodeFragment(const std::string& name) const;
    bool hasCodeFragment(const std::string& name) const;
    
    // 代码验证
    bool validateCodeFragments() const;
    std::vector<std::string> getValidationErrors() const;
    
private:
    struct CodeFragment {
        std::string name;
        std::string content;
        std::string type;
    };
    
    std::vector<CodeFragment> code_fragments_;
    
    // 合并辅助函数
    std::string generateHTML(const std::string& html) const;
    std::string generateCSS(const std::string& css) const;
    std::string generateJS(const std::string& js) const;
    std::string generateCompleteHTML(const std::string& html, const std::string& css, const std::string& js) const;
};

/**
 * 项目工作流
 */
class ProjectWorkflow {
public:
    ProjectWorkflow();
    ~ProjectWorkflow();
    
    // 工作流配置
    void setConfigurationManager(std::shared_ptr<ConfigurationManager> configManager);
    void setUseStatementManager(std::shared_ptr<UseStatementManager> useStatementManager);
    void setUnifiedScanner(std::shared_ptr<UnifiedScanner> scanner);
    void setCompilerDispatcher(std::shared_ptr<CompilerDispatcher> dispatcher);
    void setCodeMerger(std::shared_ptr<CodeMerger> merger);
    
    // 工作流执行
    std::string processFile(const std::string& filePath);
    std::string processContent(const std::string& content);
    std::string processProject(const std::string& projectPath);
    
    // 工作流步骤
    std::string scanCode(const std::string& content);
    std::string compileCode(const std::string& content, const std::string& type);
    std::string mergeCode(const std::string& html, const std::string& css, const std::string& js);
    
    // 工作流状态
    bool isProcessing() const;
    void setProcessing(bool processing);
    
    // 工作流配置
    void setDefaultHTML5Structure(bool enabled);
    void setOutputFormat(const std::string& format);
    void setMinifyOutput(bool enabled);
    
    // 工作流验证
    bool validateWorkflow() const;
    std::vector<std::string> getValidationErrors() const;
    
    // 清理
    void clear();
    void reset();
    
private:
    std::shared_ptr<ConfigurationManager> config_manager_;
    std::shared_ptr<UseStatementManager> use_statement_manager_;
    std::shared_ptr<UnifiedScanner> scanner_;
    std::shared_ptr<CompilerDispatcher> dispatcher_;
    std::shared_ptr<CodeMerger> merger_;
    
    bool processing_;
    bool default_html5_structure_;
    std::string output_format_;
    bool minify_output_;
    
    // 工作流辅助函数
    std::string processConfiguration(const std::string& content) const;
    std::string processUseStatements(const std::string& content) const;
    std::string processScanning(const std::string& content) const;
    std::string processCompilation(const std::string& content, const std::string& type) const;
    std::string processMerging(const std::string& html, const std::string& css, const std::string& js) const;
    
    // 验证辅助函数
    bool validateConfiguration() const;
    bool validateUseStatements() const;
    bool validateScanning() const;
    bool validateCompilation() const;
    bool validateMerging() const;
};

/**
 * 项目工作流管理器
 */
class ProjectWorkflowManager {
public:
    ProjectWorkflowManager();
    ~ProjectWorkflowManager();
    
    // 工作流管理
    void addWorkflow(const std::string& name, std::shared_ptr<ProjectWorkflow> workflow);
    void removeWorkflow(const std::string& name);
    std::shared_ptr<ProjectWorkflow> getWorkflow(const std::string& name) const;
    std::vector<std::string> getWorkflowNames() const;
    
    // 默认工作流
    std::shared_ptr<ProjectWorkflow> getDefaultWorkflow() const;
    void setDefaultWorkflow(const std::string& name);
    
    // 工作流执行
    std::string executeWorkflow(const std::string& name, const std::string& content);
    std::string executeDefaultWorkflow(const std::string& content);
    
    // 工作流配置
    void setGlobalConfiguration(std::shared_ptr<ConfigurationManager> configManager);
    void setGlobalUseStatementManager(std::shared_ptr<UseStatementManager> useStatementManager);
    void setGlobalUnifiedScanner(std::shared_ptr<UnifiedScanner> scanner);
    void setGlobalCompilerDispatcher(std::shared_ptr<CompilerDispatcher> dispatcher);
    void setGlobalCodeMerger(std::shared_ptr<CodeMerger> merger);
    
    // 清理
    void clear();
    void clearWorkflows();
    
private:
    std::map<std::string, std::shared_ptr<ProjectWorkflow>> workflows_;
    std::string default_workflow_name_;
    
    std::shared_ptr<ConfigurationManager> global_config_manager_;
    std::shared_ptr<UseStatementManager> global_use_statement_manager_;
    std::shared_ptr<UnifiedScanner> global_scanner_;
    std::shared_ptr<CompilerDispatcher> global_dispatcher_;
    std::shared_ptr<CodeMerger> global_merger_;
    
    // 管理辅助函数
    void applyGlobalConfiguration(std::shared_ptr<ProjectWorkflow> workflow) const;
    void createDefaultWorkflow();
};

} // namespace CHTL