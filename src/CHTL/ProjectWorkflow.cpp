#include "CHTL/ProjectWorkflow.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

// CompilerDispatcher 实现
CompilerDispatcher::CompilerDispatcher() = default;

CompilerDispatcher::~CompilerDispatcher() = default;

void CompilerDispatcher::registerCompiler(const std::string& name, std::shared_ptr<CHTLCompiler> compiler) {
    compilers_[name] = compiler;
}

void CompilerDispatcher::registerJSCompiler(const std::string& name, std::shared_ptr<CHTLJSCompiler> compiler) {
    js_compilers_[name] = compiler;
}

void CompilerDispatcher::registerCSSCompiler(const std::string& name, std::function<std::string(const std::string&)> compiler) {
    css_compilers_[name] = compiler;
}

void CompilerDispatcher::registerJSCompiler(const std::string& name, std::function<std::string(const std::string&)> compiler) {
    js_compilers_[name] = compiler;
}

std::shared_ptr<CHTLCompiler> CompilerDispatcher::getCompiler(const std::string& name) const {
    auto it = compilers_.find(name);
    if (it != compilers_.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<CHTLJSCompiler> CompilerDispatcher::getJSCompiler(const std::string& name) const {
    auto it = js_compilers_.find(name);
    if (it != js_compilers_.end()) {
        return it->second;
    }
    return nullptr;
}

std::function<std::string(const std::string&)> CompilerDispatcher::getCSSCompiler(const std::string& name) const {
    auto it = css_compilers_.find(name);
    if (it != css_compilers_.end()) {
        return it->second;
    }
    return nullptr;
}

std::function<std::string(const std::string&)> CompilerDispatcher::getJSCompiler(const std::string& name) const {
    auto it = js_compilers_.find(name);
    if (it != js_compilers_.end()) {
        return it->second;
    }
    return nullptr;
}

std::string CompilerDispatcher::dispatchCompilation(const std::string& content, const std::string& type) {
    if (type == "chtl") {
        auto compiler = getCompiler("default");
        if (compiler) {
            return compiler->compile(content);
        }
    } else if (type == "chtljs") {
        auto compiler = getJSCompiler("default");
        if (compiler) {
            return compiler->compile(content);
        }
    } else if (type == "css") {
        auto compiler = getCSSCompiler("default");
        if (compiler) {
            return compiler(content);
        }
    } else if (type == "js") {
        auto compiler = getJSCompiler("default");
        if (compiler) {
            return compiler(content);
        }
    }
    
    return content;
}

std::string CompilerDispatcher::dispatchFileCompilation(const std::string& filePath, const std::string& type) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return "";
    }
    
    std::ostringstream oss;
    oss << file.rdbuf();
    return dispatchCompilation(oss.str(), type);
}

std::vector<std::string> CompilerDispatcher::getCompilerNames() const {
    std::vector<std::string> names;
    for (const auto& pair : compilers_) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> CompilerDispatcher::getJSCompilerNames() const {
    std::vector<std::string> names;
    for (const auto& pair : js_compilers_) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> CompilerDispatcher::getCSSCompilerNames() const {
    std::vector<std::string> names;
    for (const auto& pair : css_compilers_) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> CompilerDispatcher::getJSCompilerNames() const {
    std::vector<std::string> names;
    for (const auto& pair : js_compilers_) {
        names.push_back(pair.first);
    }
    return names;
}

void CompilerDispatcher::clear() {
    clearCompilers();
    clearJSCompilers();
    clearCSSCompilers();
    clearJSCompilers();
}

void CompilerDispatcher::clearCompilers() {
    compilers_.clear();
}

void CompilerDispatcher::clearJSCompilers() {
    js_compilers_.clear();
}

void CompilerDispatcher::clearCSSCompilers() {
    css_compilers_.clear();
}

void CompilerDispatcher::clearJSCompilers() {
    js_compilers_.clear();
}

std::string CompilerDispatcher::determineCompilerType(const std::string& content) const {
    if (content.find("{{") != std::string::npos) {
        return "chtljs";
    } else if (content.find("fileloader") != std::string::npos) {
        return "chtljs";
    } else if (content.find("animate") != std::string::npos) {
        return "chtljs";
    } else if (content.find("route") != std::string::npos) {
        return "chtljs";
    } else if (content.find("$") != std::string::npos) {
        return "chtljs";
    } else if (content.find("style") != std::string::npos) {
        return "css";
    } else if (content.find("script") != std::string::npos) {
        return "js";
    } else {
        return "chtl";
    }
}

std::string CompilerDispatcher::determineFileType(const std::string& filePath) const {
    size_t pos = filePath.find_last_of('.');
    if (pos != std::string::npos) {
        std::string extension = filePath.substr(pos + 1);
        if (extension == "chtl") {
            return "chtl";
        } else if (extension == "cjjs") {
            return "chtljs";
        } else if (extension == "css") {
            return "css";
        } else if (extension == "js") {
            return "js";
        }
    }
    return "chtl";
}

// CodeMerger 实现
CodeMerger::CodeMerger() = default;

CodeMerger::~CodeMerger() = default;

void CodeMerger::addCodeFragment(const std::string& name, const std::string& content, const std::string& type) {
    CodeFragment fragment;
    fragment.name = name;
    fragment.content = content;
    fragment.type = type;
    code_fragments_.push_back(fragment);
}

void CodeMerger::removeCodeFragment(const std::string& name) {
    code_fragments_.erase(
        std::remove_if(code_fragments_.begin(), code_fragments_.end(),
            [&name](const CodeFragment& fragment) {
                return fragment.name == name;
            }),
        code_fragments_.end()
    );
}

void CodeMerger::clearCodeFragments() {
    code_fragments_.clear();
}

std::string CodeMerger::mergeCode(const std::string& html, const std::string& css, const std::string& js) const {
    return generateCompleteHTML(html, css, js);
}

std::string CodeMerger::mergeCodeFragments() const {
    std::string html, css, js;
    
    for (const auto& fragment : code_fragments_) {
        if (fragment.type == "html") {
            html += fragment.content + "\n";
        } else if (fragment.type == "css") {
            css += fragment.content + "\n";
        } else if (fragment.type == "js") {
            js += fragment.content + "\n";
        }
    }
    
    return mergeCode(html, css, js);
}

std::string CodeMerger::mergeCodeFragmentsByType(const std::string& type) const {
    std::ostringstream oss;
    
    for (const auto& fragment : code_fragments_) {
        if (fragment.type == type) {
            oss << fragment.content << "\n";
        }
    }
    
    return oss.str();
}

std::vector<std::string> CodeMerger::getCodeFragmentNames() const {
    std::vector<std::string> names;
    for (const auto& fragment : code_fragments_) {
        names.push_back(fragment.name);
    }
    return names;
}

std::vector<std::string> CodeMerger::getCodeFragmentNamesByType(const std::string& type) const {
    std::vector<std::string> names;
    for (const auto& fragment : code_fragments_) {
        if (fragment.type == type) {
            names.push_back(fragment.name);
        }
    }
    return names;
}

std::string CodeMerger::getCodeFragment(const std::string& name) const {
    auto it = std::find_if(code_fragments_.begin(), code_fragments_.end(),
        [&name](const CodeFragment& fragment) {
            return fragment.name == name;
        });
    
    if (it != code_fragments_.end()) {
        return it->content;
    }
    
    return "";
}

bool CodeMerger::hasCodeFragment(const std::string& name) const {
    return std::any_of(code_fragments_.begin(), code_fragments_.end(),
        [&name](const CodeFragment& fragment) {
            return fragment.name == name;
        });
}

bool CodeMerger::validateCodeFragments() const {
    for (const auto& fragment : code_fragments_) {
        if (fragment.name.empty() || fragment.content.empty()) {
            return false;
        }
    }
    return true;
}

std::vector<std::string> CodeMerger::getValidationErrors() const {
    std::vector<std::string> errors;
    
    for (const auto& fragment : code_fragments_) {
        if (fragment.name.empty()) {
            errors.push_back("Code fragment name cannot be empty");
        }
        if (fragment.content.empty()) {
            errors.push_back("Code fragment content cannot be empty");
        }
    }
    
    return errors;
}

std::string CodeMerger::generateHTML(const std::string& html) const {
    return html;
}

std::string CodeMerger::generateCSS(const std::string& css) const {
    return "<style>\n" + css + "\n</style>";
}

std::string CodeMerger::generateJS(const std::string& js) const {
    return "<script>\n" + js + "\n</script>";
}

std::string CodeMerger::generateCompleteHTML(const std::string& html, const std::string& css, const std::string& js) const {
    std::ostringstream oss;
    
    oss << "<!DOCTYPE html>" << std::endl;
    oss << "<html>" << std::endl;
    oss << "<head>" << std::endl;
    oss << "    <meta charset=\"UTF-8\">" << std::endl;
    oss << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" << std::endl;
    oss << "    <title>CHTL Generated Page</title>" << std::endl;
    
    if (!css.empty()) {
        oss << "    " << generateCSS(css) << std::endl;
    }
    
    oss << "</head>" << std::endl;
    oss << "<body>" << std::endl;
    oss << html << std::endl;
    
    if (!js.empty()) {
        oss << generateJS(js) << std::endl;
    }
    
    oss << "</body>" << std::endl;
    oss << "</html>" << std::endl;
    
    return oss.str();
}

// ProjectWorkflow 实现
ProjectWorkflow::ProjectWorkflow() 
    : processing_(false), default_html5_structure_(true), output_format_("html"), minify_output_(false) {
}

ProjectWorkflow::~ProjectWorkflow() = default;

void ProjectWorkflow::setConfigurationManager(std::shared_ptr<ConfigurationManager> configManager) {
    config_manager_ = configManager;
}

void ProjectWorkflow::setUseStatementManager(std::shared_ptr<UseStatementManager> useStatementManager) {
    use_statement_manager_ = useStatementManager;
}

void ProjectWorkflow::setUnifiedScanner(std::shared_ptr<UnifiedScanner> scanner) {
    scanner_ = scanner;
}

void ProjectWorkflow::setCompilerDispatcher(std::shared_ptr<CompilerDispatcher> dispatcher) {
    dispatcher_ = dispatcher;
}

void ProjectWorkflow::setCodeMerger(std::shared_ptr<CodeMerger> merger) {
    merger_ = merger;
}

std::string ProjectWorkflow::processFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return "";
    }
    
    std::ostringstream oss;
    oss << file.rdbuf();
    return processContent(oss.str());
}

std::string ProjectWorkflow::processContent(const std::string& content) {
    setProcessing(true);
    
    try {
        // 1. 处理配置
        std::string processedContent = processConfiguration(content);
        
        // 2. 处理Use语句
        processedContent = processUseStatements(processedContent);
        
        // 3. 扫描代码
        std::string scannedContent = processScanning(processedContent);
        
        // 4. 编译代码
        std::string compiledContent = processCompilation(scannedContent, "chtl");
        
        // 5. 合并代码
        std::string mergedContent = processMerging(compiledContent, "", "");
        
        setProcessing(false);
        return mergedContent;
    } catch (const std::exception& e) {
        setProcessing(false);
        throw;
    }
}

std::string ProjectWorkflow::processProject(const std::string& projectPath) {
    // 处理整个项目
    std::ostringstream oss;
    
    // 这里应该遍历项目目录，处理所有CHTL文件
    // 简化实现，只处理单个文件
    
    return oss.str();
}

std::string ProjectWorkflow::scanCode(const std::string& content) {
    if (!scanner_) {
        return content;
    }
    
    return scanner_->scan(content);
}

std::string ProjectWorkflow::compileCode(const std::string& content, const std::string& type) {
    if (!dispatcher_) {
        return content;
    }
    
    return dispatcher_->dispatchCompilation(content, type);
}

std::string ProjectWorkflow::mergeCode(const std::string& html, const std::string& css, const std::string& js) {
    if (!merger_) {
        return html;
    }
    
    return merger_->mergeCode(html, css, js);
}

bool ProjectWorkflow::isProcessing() const {
    return processing_;
}

void ProjectWorkflow::setProcessing(bool processing) {
    processing_ = processing;
}

void ProjectWorkflow::setDefaultHTML5Structure(bool enabled) {
    default_html5_structure_ = enabled;
}

void ProjectWorkflow::setOutputFormat(const std::string& format) {
    output_format_ = format;
}

void ProjectWorkflow::setMinifyOutput(bool enabled) {
    minify_output_ = enabled;
}

bool ProjectWorkflow::validateWorkflow() const {
    return validateConfiguration() && validateUseStatements() && 
           validateScanning() && validateCompilation() && validateMerging();
}

std::vector<std::string> ProjectWorkflow::getValidationErrors() const {
    std::vector<std::string> errors;
    
    if (!validateConfiguration()) {
        errors.push_back("Configuration validation failed");
    }
    
    if (!validateUseStatements()) {
        errors.push_back("Use statements validation failed");
    }
    
    if (!validateScanning()) {
        errors.push_back("Scanning validation failed");
    }
    
    if (!validateCompilation()) {
        errors.push_back("Compilation validation failed");
    }
    
    if (!validateMerging()) {
        errors.push_back("Merging validation failed");
    }
    
    return errors;
}

void ProjectWorkflow::clear() {
    config_manager_.reset();
    use_statement_manager_.reset();
    scanner_.reset();
    dispatcher_.reset();
    merger_.reset();
}

void ProjectWorkflow::reset() {
    clear();
    processing_ = false;
    default_html5_structure_ = true;
    output_format_ = "html";
    minify_output_ = false;
}

std::string ProjectWorkflow::processConfiguration(const std::string& content) const {
    if (!config_manager_) {
        return content;
    }
    
    // 处理配置组
    return content;
}

std::string ProjectWorkflow::processUseStatements(const std::string& content) const {
    if (!use_statement_manager_) {
        return content;
    }
    
    // 处理Use语句
    return content;
}

std::string ProjectWorkflow::processScanning(const std::string& content) const {
    if (!scanner_) {
        return content;
    }
    
    return scanner_->scan(content);
}

std::string ProjectWorkflow::processCompilation(const std::string& content, const std::string& type) const {
    if (!dispatcher_) {
        return content;
    }
    
    return dispatcher_->dispatchCompilation(content, type);
}

std::string ProjectWorkflow::processMerging(const std::string& html, const std::string& css, const std::string& js) const {
    if (!merger_) {
        return html;
    }
    
    return merger_->mergeCode(html, css, js);
}

bool ProjectWorkflow::validateConfiguration() const {
    return config_manager_ != nullptr;
}

bool ProjectWorkflow::validateUseStatements() const {
    return use_statement_manager_ != nullptr;
}

bool ProjectWorkflow::validateScanning() const {
    return scanner_ != nullptr;
}

bool ProjectWorkflow::validateCompilation() const {
    return dispatcher_ != nullptr;
}

bool ProjectWorkflow::validateMerging() const {
    return merger_ != nullptr;
}

// ProjectWorkflowManager 实现
ProjectWorkflowManager::ProjectWorkflowManager() = default;

ProjectWorkflowManager::~ProjectWorkflowManager() = default;

void ProjectWorkflowManager::addWorkflow(const std::string& name, std::shared_ptr<ProjectWorkflow> workflow) {
    if (workflow) {
        workflows_[name] = workflow;
        applyGlobalConfiguration(workflow);
    }
}

void ProjectWorkflowManager::removeWorkflow(const std::string& name) {
    workflows_.erase(name);
}

std::shared_ptr<ProjectWorkflow> ProjectWorkflowManager::getWorkflow(const std::string& name) const {
    auto it = workflows_.find(name);
    if (it != workflows_.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::string> ProjectWorkflowManager::getWorkflowNames() const {
    std::vector<std::string> names;
    for (const auto& pair : workflows_) {
        names.push_back(pair.first);
    }
    return names;
}

std::shared_ptr<ProjectWorkflow> ProjectWorkflowManager::getDefaultWorkflow() const {
    if (default_workflow_name_.empty()) {
        return nullptr;
    }
    
    return getWorkflow(default_workflow_name_);
}

void ProjectWorkflowManager::setDefaultWorkflow(const std::string& name) {
    if (workflows_.find(name) != workflows_.end()) {
        default_workflow_name_ = name;
    }
}

std::string ProjectWorkflowManager::executeWorkflow(const std::string& name, const std::string& content) {
    auto workflow = getWorkflow(name);
    if (workflow) {
        return workflow->processContent(content);
    }
    return content;
}

std::string ProjectWorkflowManager::executeDefaultWorkflow(const std::string& content) {
    auto workflow = getDefaultWorkflow();
    if (workflow) {
        return workflow->processContent(content);
    }
    return content;
}

void ProjectWorkflowManager::setGlobalConfiguration(std::shared_ptr<ConfigurationManager> configManager) {
    global_config_manager_ = configManager;
}

void ProjectWorkflowManager::setGlobalUseStatementManager(std::shared_ptr<UseStatementManager> useStatementManager) {
    global_use_statement_manager_ = useStatementManager;
}

void ProjectWorkflowManager::setGlobalUnifiedScanner(std::shared_ptr<UnifiedScanner> scanner) {
    global_scanner_ = scanner;
}

void ProjectWorkflowManager::setGlobalCompilerDispatcher(std::shared_ptr<CompilerDispatcher> dispatcher) {
    global_dispatcher_ = dispatcher;
}

void ProjectWorkflowManager::setGlobalCodeMerger(std::shared_ptr<CodeMerger> merger) {
    global_merger_ = merger;
}

void ProjectWorkflowManager::clear() {
    clearWorkflows();
    global_config_manager_.reset();
    global_use_statement_manager_.reset();
    global_scanner_.reset();
    global_dispatcher_.reset();
    global_merger_.reset();
}

void ProjectWorkflowManager::clearWorkflows() {
    workflows_.clear();
    default_workflow_name_.clear();
}

void ProjectWorkflowManager::applyGlobalConfiguration(std::shared_ptr<ProjectWorkflow> workflow) const {
    if (!workflow) {
        return;
    }
    
    if (global_config_manager_) {
        workflow->setConfigurationManager(global_config_manager_);
    }
    
    if (global_use_statement_manager_) {
        workflow->setUseStatementManager(global_use_statement_manager_);
    }
    
    if (global_scanner_) {
        workflow->setUnifiedScanner(global_scanner_);
    }
    
    if (global_dispatcher_) {
        workflow->setCompilerDispatcher(global_dispatcher_);
    }
    
    if (global_merger_) {
        workflow->setCodeMerger(global_merger_);
    }
}

void ProjectWorkflowManager::createDefaultWorkflow() {
    auto workflow = std::make_shared<ProjectWorkflow>();
    addWorkflow("default", workflow);
    setDefaultWorkflow("default");
}

} // namespace CHTL