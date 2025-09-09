#include "CHTL/CHTLContext.h"
#include <fstream>
#include <sstream>

namespace CHTL {

// CHTLContext 实现
CHTLContext::CHTLContext() {
    initializeDefaultConfig();
    initializeDefaultPaths();
}

void CHTLContext::initializeDefaultConfig() {
    config_.debugMode = false;
    config_.indexInitialCount = 0;
    config_.disableStyleAutoAddClass = false;
    config_.disableStyleAutoAddId = false;
    config_.disableScriptAutoAddClass = true;
    config_.disableScriptAutoAddId = true;
    config_.disableDefaultNamespace = false;
    config_.disableCustomOriginType = false;
    config_.disableNameGroup = false;
    
    // 设置默认关键字
    config_.customStyleKeywords = {"@Style", "@style", "@CSS", "@Css", "@css"};
    config_.customElementKeyword = "@Element";
    config_.customVarKeyword = "@Var";
    config_.templateStyleKeyword = "@Style";
    config_.templateElementKeyword = "@Element";
    config_.templateVarKeyword = "@Var";
    config_.originHtmlKeyword = "@Html";
    config_.originStyleKeyword = "@Style";
    config_.originJavascriptKeyword = "@JavaScript";
    config_.importHtmlKeyword = "@Html";
    config_.importStyleKeyword = "@Style";
    config_.importJavascriptKeyword = "@JavaScript";
    config_.importChtlKeyword = "@Chtl";
    config_.importCjmodKeyword = "@CJmod";
    config_.keywordInherit = "inherit";
    config_.keywordDelete = "delete";
    config_.keywordInsert = "insert";
    config_.keywordAfter = "after";
    config_.keywordBefore = "before";
    config_.keywordReplace = "replace";
    config_.keywordAtTop = "at top";
    config_.keywordAtBottom = "at bottom";
    config_.keywordFrom = "from";
    config_.keywordAs = "as";
    config_.keywordExcept = "except";
    config_.keywordUse = "use";
    config_.keywordHtml5 = "html5";
    config_.keywordText = "text";
    config_.keywordStyle = "style";
    config_.keywordScript = "script";
    config_.keywordCustom = "[Custom]";
    config_.keywordTemplate = "[Template]";
    config_.keywordOrigin = "[Origin]";
    config_.keywordImport = "[Import]";
    config_.keywordNamespace = "[Namespace]";
    config_.keywordConfiguration = "[Configuration]";
    config_.optionCount = 3;
}

void CHTLContext::initializeDefaultPaths() {
    modulePaths_.push_back("./module");
    modulePaths_.push_back("./modules");
    importPaths_.push_back("./");
    importPaths_.push_back("./src");
    importPaths_.push_back("./include");
}

void CHTLContext::addTemplate(const String& name, std::shared_ptr<TemplateNode> template_) {
    templates_[name] = template_;
}

std::shared_ptr<TemplateNode> CHTLContext::getTemplate(const String& name) const {
    auto it = templates_.find(name);
    return (it != templates_.end()) ? it->second : nullptr;
}

bool CHTLContext::hasTemplate(const String& name) const {
    return templates_.find(name) != templates_.end();
}

void CHTLContext::removeTemplate(const String& name) {
    templates_.erase(name);
}

void CHTLContext::addCustom(const String& name, std::shared_ptr<CustomNode> custom) {
    customs_[name] = custom;
}

std::shared_ptr<CustomNode> CHTLContext::getCustom(const String& name) const {
    auto it = customs_.find(name);
    return (it != customs_.end()) ? it->second : nullptr;
}

bool CHTLContext::hasCustom(const String& name) const {
    return customs_.find(name) != customs_.end();
}

void CHTLContext::removeCustom(const String& name) {
    customs_.erase(name);
}

void CHTLContext::setVariable(const String& name, const String& value) {
    variables_[name] = value;
}

String CHTLContext::getVariable(const String& name) const {
    auto it = variables_.find(name);
    return (it != variables_.end()) ? it->second : "";
}

bool CHTLContext::hasVariable(const String& name) const {
    return variables_.find(name) != variables_.end();
}

void CHTLContext::removeVariable(const String& name) {
    variables_.erase(name);
}

void CHTLContext::addNamespace(const String& name, std::shared_ptr<NamespaceNode> namespace_) {
    namespaces_[name] = namespace_;
}

std::shared_ptr<NamespaceNode> CHTLContext::getNamespace(const String& name) const {
    auto it = namespaces_.find(name);
    return (it != namespaces_.end()) ? it->second : nullptr;
}

bool CHTLContext::hasNamespace(const String& name) const {
    return namespaces_.find(name) != namespaces_.end();
}

void CHTLContext::removeNamespace(const String& name) {
    namespaces_.erase(name);
}

void CHTLContext::addImport(const String& path, std::shared_ptr<CHTLNode> import_) {
    imports_[path] = import_;
}

std::shared_ptr<CHTLNode> CHTLContext::getImport(const String& path) const {
    auto it = imports_.find(path);
    return (it != imports_.end()) ? it->second : nullptr;
}

bool CHTLContext::hasImport(const String& path) const {
    return imports_.find(path) != imports_.end();
}

void CHTLContext::removeImport(const String& path) {
    imports_.erase(path);
}

void CHTLContext::addError(const ErrorInfo& error) {
    errors_.push_back(error);
}

void CHTLContext::addError(const String& message, const Position& pos, const String& severity) {
    errors_.emplace_back(message, pos, severity);
}

void CHTLContext::addWarning(const String& message, const Position& pos) {
    warnings_.emplace_back(message, pos, "warning");
}

void CHTLContext::clearErrors() {
    errors_.clear();
}

void CHTLContext::clearWarnings() {
    warnings_.clear();
}

void CHTLContext::addModule(const String& name, const ModuleInfo& info) {
    modules_[name] = info;
}

const ModuleInfo* CHTLContext::getModule(const String& name) const {
    auto it = modules_.find(name);
    return (it != modules_.end()) ? &it->second : nullptr;
}

bool CHTLContext::hasModule(const String& name) const {
    return modules_.find(name) != modules_.end();
}

void CHTLContext::removeModule(const String& name) {
    modules_.erase(name);
}

void CHTLContext::addExport(const String& name, const ExportInfo& info) {
    exports_[name] = info;
}

const ExportInfo* CHTLContext::getExport(const String& name) const {
    auto it = exports_.find(name);
    return (it != exports_.end()) ? &it->second : nullptr;
}

bool CHTLContext::hasExport(const String& name) const {
    return exports_.find(name) != exports_.end();
}

void CHTLContext::removeExport(const String& name) {
    exports_.erase(name);
}

void CHTLContext::setCurrentFile(const String& file) {
    currentFile_ = file;
}

String CHTLContext::resolvePath(const String& path) const {
    if (isAbsolutePath(path)) {
        return path;
    }
    
    // 相对路径解析
    String baseDir = getDirectory(currentFile_);
    return joinPath({baseDir, path});
}

String CHTLContext::resolveModulePath(const String& moduleName) const {
    for (const auto& modulePath : modulePaths_) {
        String fullPath = joinPath({modulePath, moduleName});
        if (std::ifstream(fullPath).good()) {
            return fullPath;
        }
    }
    return "";
}

String CHTLContext::resolveImportPath(const String& importPath) const {
    if (isAbsolutePath(importPath)) {
        return importPath;
    }
    
    for (const auto& importPath : importPaths_) {
        String fullPath = joinPath({importPath, importPath});
        if (std::ifstream(fullPath).good()) {
            return fullPath;
        }
    }
    
    return resolvePath(importPath);
}

void CHTLContext::clear() {
    clearTemplates();
    clearCustoms();
    clearVariables();
    clearNamespaces();
    clearImports();
    clearModules();
    clearExports();
    clearErrors();
    clearWarnings();
    rootNode_.reset();
    currentFile_.clear();
}

void CHTLContext::clearTemplates() {
    templates_.clear();
}

void CHTLContext::clearCustoms() {
    customs_.clear();
}

void CHTLContext::clearVariables() {
    variables_.clear();
}

void CHTLContext::clearNamespaces() {
    namespaces_.clear();
}

void CHTLContext::clearImports() {
    imports_.clear();
}

void CHTLContext::clearModules() {
    modules_.clear();
}

void CHTLContext::clearExports() {
    exports_.clear();
}

bool CHTLContext::validate() const {
    return validateTemplates() && validateCustoms() && validateNamespaces() && validateImports();
}

bool CHTLContext::validateTemplates() const {
    for (const auto& template_ : templates_) {
        if (template_.first.empty()) {
            return false;
        }
    }
    return true;
}

bool CHTLContext::validateCustoms() const {
    for (const auto& custom : customs_) {
        if (custom.first.empty()) {
            return false;
        }
    }
    return true;
}

bool CHTLContext::validateNamespaces() const {
    for (const auto& namespace_ : namespaces_) {
        if (namespace_.first.empty()) {
            return false;
        }
    }
    return true;
}

bool CHTLContext::validateImports() const {
    for (const auto& import_ : imports_) {
        if (import_.first.empty()) {
            return false;
        }
    }
    return true;
}

void CHTLContext::printDebugInfo() const {
    printTemplates();
    printCustoms();
    printVariables();
    printNamespaces();
    printImports();
    printModules();
    printExports();
}

void CHTLContext::printTemplates() const {
    std::cout << "Templates:\n";
    for (const auto& template_ : templates_) {
        std::cout << "  " << template_.first << "\n";
    }
}

void CHTLContext::printCustoms() const {
    std::cout << "Customs:\n";
    for (const auto& custom : customs_) {
        std::cout << "  " << custom.first << "\n";
    }
}

void CHTLContext::printVariables() const {
    std::cout << "Variables:\n";
    for (const auto& var : variables_) {
        std::cout << "  " << var.first << " = " << var.second << "\n";
    }
}

void CHTLContext::printNamespaces() const {
    std::cout << "Namespaces:\n";
    for (const auto& namespace_ : namespaces_) {
        std::cout << "  " << namespace_.first << "\n";
    }
}

void CHTLContext::printImports() const {
    std::cout << "Imports:\n";
    for (const auto& import_ : imports_) {
        std::cout << "  " << import_.first << "\n";
    }
}

void CHTLContext::printModules() const {
    std::cout << "Modules:\n";
    for (const auto& module : modules_) {
        std::cout << "  " << module.first << " v" << module.second.version << "\n";
    }
}

void CHTLContext::printExports() const {
    std::cout << "Exports:\n";
    for (const auto& export_ : exports_) {
        std::cout << "  " << export_.first << "\n";
    }
}

StringList CHTLContext::splitPath(const String& path) const {
    StringList parts;
    String current;
    
    for (char c : path) {
        if (c == '/' || c == '\\') {
            if (!current.empty()) {
                parts.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        parts.push_back(current);
    }
    
    return parts;
}

String CHTLContext::joinPath(const StringList& parts) const {
    if (parts.empty()) return "";
    
    String result = parts[0];
    for (size_t i = 1; i < parts.size(); ++i) {
        result += "/" + parts[i];
    }
    
    return result;
}

bool CHTLContext::isAbsolutePath(const String& path) const {
    if (path.empty()) return false;
    
    // Unix-like 绝对路径
    if (path[0] == '/') return true;
    
    // Windows 绝对路径
    if (path.length() >= 2 && path[1] == ':') return true;
    
    return false;
}

String CHTLContext::getDirectory(const String& file) const {
    size_t lastSlash = file.find_last_of("/\\");
    if (lastSlash == String::npos) {
        return ".";
    }
    return file.substr(0, lastSlash);
}

String CHTLContext::getFileName(const String& file) const {
    size_t lastSlash = file.find_last_of("/\\");
    if (lastSlash == String::npos) {
        return file;
    }
    return file.substr(lastSlash + 1);
}

String CHTLContext::getFileExtension(const String& file) const {
    String fileName = getFileName(file);
    size_t lastDot = fileName.find_last_of('.');
    if (lastDot == String::npos) {
        return "";
    }
    return fileName.substr(lastDot + 1);
}

} // namespace CHTL