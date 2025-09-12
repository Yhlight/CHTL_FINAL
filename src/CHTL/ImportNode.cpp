#include "CHTL/ImportNode.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace CHTL {

// ImportNode 实现
ImportNode::ImportNode(ImportType type, const std::string& path)
    : CHTLNode(NodeType::IMPORT), import_type_(type), import_path_(path), 
      import_mode_(ImportMode::EXACT) {
}

ImportNode::ImportType ImportNode::getImportType() const {
    return import_type_;
}

void ImportNode::setImportType(ImportType type) {
    import_type_ = type;
}

std::string ImportNode::getImportPath() const {
    return import_path_;
}

void ImportNode::setImportPath(const std::string& path) {
    import_path_ = path;
}

ImportNode::ImportMode ImportNode::getImportMode() const {
    return import_mode_;
}

void ImportNode::setImportMode(ImportMode mode) {
    import_mode_ = mode;
}

std::string ImportNode::getImportName() const {
    return import_name_;
}

void ImportNode::setImportName(const std::string& name) {
    import_name_ = name;
}

std::string ImportNode::getImportAlias() const {
    return import_alias_;
}

void ImportNode::setImportAlias(const std::string& alias) {
    import_alias_ = alias;
}

std::string ImportNode::getNamespace() const {
    return namespace_;
}

void ImportNode::setNamespace(const std::string& namespaceName) {
    namespace_ = namespaceName;
}

std::shared_ptr<CHTLNode> ImportNode::clone() const {
    auto cloned = std::make_shared<ImportNode>(import_type_, import_path_);
    cloned->import_mode_ = import_mode_;
    cloned->import_name_ = import_name_;
    cloned->import_alias_ = import_alias_;
    cloned->namespace_ = namespace_;
    
    cloned->setContent(content_);
    cloned->setPosition(line_, column_);
    
    return cloned;
}

bool ImportNode::validate() const {
    // 基本验证：导入路径不能为空
    if (import_path_.empty()) {
        return false;
    }
    
    // 根据导入类型进行特定验证
    switch (import_type_) {
        case ImportType::HTML:
        case ImportType::CSS:
        case ImportType::JS:
        case ImportType::CHTL:
        case ImportType::CJMOD:
            // 这些类型需要有效的文件路径
            return !import_path_.empty();
        case ImportType::TEMPLATE:
        case ImportType::CUSTOM:
        case ImportType::ORIGIN:
        case ImportType::CONFIGURATION:
            // 这些类型需要有效的名称
            return !import_name_.empty();
        default:
            return false;
    }
}

std::string ImportNode::generateHTML() const {
    // 导入节点本身不生成HTML，而是被处理
    return "";
}

// ImportManager 实现
ImportManager::ImportManager() : base_path_(".") {
    // 添加默认搜索路径
    search_paths_.push_back(".");
    search_paths_.push_back("./module");
    search_paths_.push_back("../module");
}

ImportManager::~ImportManager() = default;

bool ImportManager::processImport(std::shared_ptr<ImportNode> importNode) {
    if (!importNode || !importNode->validate()) {
        return false;
    }
    
    switch (importNode->getImportType()) {
        case ImportType::HTML:
            return processHTMLImport(importNode->getImportPath(), importNode->getImportAlias());
        case ImportType::CSS:
            return processCSSImport(importNode->getImportPath(), importNode->getImportAlias());
        case ImportType::JS:
            return processJSImport(importNode->getImportPath(), importNode->getImportAlias());
        case ImportType::CHTL:
            return processCHTLImport(importNode->getImportPath(), importNode->getImportAlias());
        case ImportType::CJMOD:
            return processCJMODImport(importNode->getImportPath(), importNode->getImportAlias());
        case ImportType::TEMPLATE:
            return processTemplateImport(ImportType::TEMPLATE, importNode->getImportName(), 
                                       importNode->getImportPath(), importNode->getImportAlias());
        case ImportType::CUSTOM:
            return processTemplateImport(ImportType::CUSTOM, importNode->getImportName(), 
                                       importNode->getImportPath(), importNode->getImportAlias());
        case ImportType::ORIGIN:
            return processTemplateImport(ImportType::ORIGIN, importNode->getImportName(), 
                                       importNode->getImportPath(), importNode->getImportAlias());
        case ImportType::CONFIGURATION:
            return processTemplateImport(ImportType::CONFIGURATION, importNode->getImportName(), 
                                       importNode->getImportPath(), importNode->getImportAlias());
        default:
            return false;
    }
}

bool ImportManager::processHTMLImport(const std::string& path, const std::string& alias) {
    std::string resolvedPath = resolvePath(path);
    if (resolvedPath.empty() || !fileExists(resolvedPath)) {
        return false;
    }
    
    markAsImported(resolvedPath);
    return true;
}

bool ImportManager::processCSSImport(const std::string& path, const std::string& alias) {
    std::string resolvedPath = resolvePath(path);
    if (resolvedPath.empty() || !fileExists(resolvedPath)) {
        return false;
    }
    
    markAsImported(resolvedPath);
    return true;
}

bool ImportManager::processJSImport(const std::string& path, const std::string& alias) {
    std::string resolvedPath = resolvePath(path);
    if (resolvedPath.empty() || !fileExists(resolvedPath)) {
        return false;
    }
    
    markAsImported(resolvedPath);
    return true;
}

bool ImportManager::processCHTLImport(const std::string& path, const std::string& alias) {
    std::string resolvedPath = resolvePath(path);
    if (resolvedPath.empty() || !fileExists(resolvedPath)) {
        return false;
    }
    
    markAsImported(resolvedPath);
    return true;
}

bool ImportManager::processCJMODImport(const std::string& path, const std::string& alias) {
    std::string resolvedPath = resolvePath(path);
    if (resolvedPath.empty() || !fileExists(resolvedPath)) {
        return false;
    }
    
    markAsImported(resolvedPath);
    return true;
}

bool ImportManager::processTemplateImport(ImportType type, const std::string& name, 
                                        const std::string& path, const std::string& alias) {
    std::string resolvedPath = resolvePath(path);
    if (resolvedPath.empty() || !fileExists(resolvedPath)) {
        return false;
    }
    
    markAsImported(resolvedPath);
    return true;
}

bool ImportManager::processWildcardImport(ImportType type, const std::string& path, 
                                        const std::string& alias) {
    std::vector<std::string> files = expandWildcard(path);
    bool success = true;
    
    for (const auto& file : files) {
        if (!isImported(file)) {
            markAsImported(file);
        }
    }
    
    return success;
}

std::string ImportManager::resolvePath(const std::string& path) const {
    // 如果是绝对路径，直接返回
    if (isAbsolutePath(path)) {
        return fileExists(path) ? path : "";
    }
    
    // 在搜索路径中查找文件
    for (const auto& searchPath : search_paths_) {
        std::string fullPath = joinPath(searchPath, path);
        if (fileExists(fullPath)) {
            return fullPath;
        }
    }
    
    return "";
}

std::vector<std::string> ImportManager::findFiles(const std::string& pattern) const {
    std::vector<std::string> files;
    
    for (const auto& searchPath : search_paths_) {
        std::vector<std::string> foundFiles = findFilesByPattern(searchPath, pattern);
        files.insert(files.end(), foundFiles.begin(), foundFiles.end());
    }
    
    return files;
}

void ImportManager::setCurrentNamespace(const std::string& namespaceName) {
    current_namespace_ = namespaceName;
}

std::string ImportManager::getCurrentNamespace() const {
    return current_namespace_;
}

void ImportManager::addNamespaceMapping(const std::string& alias, const std::string& namespaceName) {
    namespace_mappings_[alias] = namespaceName;
}

std::string ImportManager::getNamespaceMapping(const std::string& alias) const {
    auto it = namespace_mappings_.find(alias);
    if (it != namespace_mappings_.end()) {
        return it->second;
    }
    return "";
}

bool ImportManager::isImported(const std::string& path) const {
    return imported_files_.find(path) != imported_files_.end();
}

void ImportManager::markAsImported(const std::string& path) {
    imported_files_.insert(path);
}

void ImportManager::clearImported() {
    imported_files_.clear();
}

void ImportManager::setBasePath(const std::string& path) {
    base_path_ = path;
}

std::string ImportManager::getBasePath() const {
    return base_path_;
}

void ImportManager::addSearchPath(const std::string& path) {
    search_paths_.push_back(path);
}

std::vector<std::string> ImportManager::getSearchPaths() const {
    return search_paths_;
}

void ImportManager::clear() {
    imported_files_.clear();
    namespace_mappings_.clear();
    search_paths_.clear();
    current_namespace_.clear();
}

bool ImportManager::fileExists(const std::string& path) const {
    return std::filesystem::exists(path);
}

std::string ImportManager::readFile(const std::string& path) const {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }
    
    std::ostringstream oss;
    oss << file.rdbuf();
    return oss.str();
}

std::vector<std::string> ImportManager::listFiles(const std::string& directory) const {
    std::vector<std::string> files;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().string());
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        // 忽略文件系统错误
    }
    
    return files;
}

std::vector<std::string> ImportManager::findFilesByPattern(const std::string& directory, 
                                                          const std::string& pattern) const {
    std::vector<std::string> files;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (matchesPattern(filename, pattern)) {
                    files.push_back(entry.path().string());
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        // 忽略文件系统错误
    }
    
    return files;
}

std::string ImportManager::normalizePath(const std::string& path) const {
    std::string normalized = path;
    
    // 替换反斜杠为正斜杠
    std::replace(normalized.begin(), normalized.end(), '\\', '/');
    
    // 处理相对路径
    if (normalized.find("./") == 0) {
        normalized = normalized.substr(2);
    }
    
    return normalized;
}

std::string ImportManager::joinPath(const std::string& base, const std::string& relative) const {
    if (base.empty()) {
        return relative;
    }
    
    if (relative.empty()) {
        return base;
    }
    
    std::string result = base;
    if (result.back() != '/' && relative.front() != '/') {
        result += '/';
    }
    
    result += relative;
    return result;
}

bool ImportManager::isAbsolutePath(const std::string& path) const {
    if (path.empty()) {
        return false;
    }
    
    // Windows绝对路径
    if (path.length() >= 2 && path[1] == ':') {
        return true;
    }
    
    // Unix绝对路径
    return path[0] == '/';
}

std::string ImportManager::getDirectory(const std::string& path) const {
    size_t pos = path.find_last_of('/');
    if (pos == std::string::npos) {
        return ".";
    }
    
    return path.substr(0, pos);
}

std::string ImportManager::getFileName(const std::string& path) const {
    size_t pos = path.find_last_of('/');
    if (pos == std::string::npos) {
        return path;
    }
    
    return path.substr(pos + 1);
}

std::string ImportManager::getFileExtension(const std::string& path) const {
    std::string filename = getFileName(path);
    size_t pos = filename.find_last_of('.');
    if (pos == std::string::npos) {
        return "";
    }
    
    return filename.substr(pos + 1);
}

bool ImportManager::matchesPattern(const std::string& filename, const std::string& pattern) const {
    // 简化的模式匹配实现
    if (pattern == "*") {
        return true;
    }
    
    if (pattern.find("*") == std::string::npos) {
        return filename == pattern;
    }
    
    // 处理通配符
    size_t starPos = pattern.find("*");
    if (starPos == 0) {
        // 以*开头
        std::string suffix = pattern.substr(1);
        return filename.length() >= suffix.length() && 
               filename.substr(filename.length() - suffix.length()) == suffix;
    } else if (starPos == pattern.length() - 1) {
        // 以*结尾
        std::string prefix = pattern.substr(0, starPos);
        return filename.length() >= prefix.length() && 
               filename.substr(0, prefix.length()) == prefix;
    }
    
    return false;
}

std::vector<std::string> ImportManager::expandWildcard(const std::string& pattern) const {
    std::vector<std::string> files;
    
    for (const auto& searchPath : search_paths_) {
        std::vector<std::string> foundFiles = findFilesByPattern(searchPath, pattern);
        files.insert(files.end(), foundFiles.begin(), foundFiles.end());
    }
    
    return files;
}

// NamespaceManager 实现
NamespaceManager::NamespaceManager() = default;

NamespaceManager::~NamespaceManager() = default;

void NamespaceManager::createNamespace(const std::string& name, const std::string& parent) {
    if (parent.empty()) {
        namespaces_[name] = name;
    } else {
        namespaces_[name] = parent + "::" + name;
    }
}

void NamespaceManager::deleteNamespace(const std::string& name) {
    namespaces_.erase(name);
    nested_namespaces_.erase(name);
}

bool NamespaceManager::hasNamespace(const std::string& name) const {
    return namespaces_.find(name) != namespaces_.end();
}

std::string NamespaceManager::getFullNamespace(const std::string& name) const {
    auto it = namespaces_.find(name);
    if (it != namespaces_.end()) {
        return it->second;
    }
    return name;
}

void NamespaceManager::addNestedNamespace(const std::string& parent, const std::string& child) {
    nested_namespaces_[parent].push_back(child);
}

std::vector<std::string> NamespaceManager::getNestedNamespaces(const std::string& parent) const {
    auto it = nested_namespaces_.find(parent);
    if (it != nested_namespaces_.end()) {
        return it->second;
    }
    return {};
}

void NamespaceManager::mergeNamespaces(const std::string& name1, const std::string& name2) {
    if (hasConflict(name1, name2)) {
        resolveConflict(name1, name2);
    }
}

bool NamespaceManager::hasConflict(const std::string& name1, const std::string& name2) const {
    return checkConflict(name1, name2);
}

void NamespaceManager::setDefaultNamespace(const std::string& name) {
    default_namespace_ = name;
}

std::string NamespaceManager::getDefaultNamespace() const {
    return default_namespace_;
}

std::vector<std::string> NamespaceManager::getAllNamespaces() const {
    std::vector<std::string> result;
    for (const auto& pair : namespaces_) {
        result.push_back(pair.first);
    }
    return result;
}

std::string NamespaceManager::getParentNamespace(const std::string& name) const {
    auto it = namespaces_.find(name);
    if (it != namespaces_.end()) {
        std::string fullName = it->second;
        size_t pos = fullName.find_last_of("::");
        if (pos != std::string::npos) {
            return fullName.substr(0, pos);
        }
    }
    return "";
}

void NamespaceManager::clear() {
    namespaces_.clear();
    nested_namespaces_.clear();
    default_namespace_.clear();
}

bool NamespaceManager::checkConflict(const std::string& name1, const std::string& name2) const {
    // 简化的冲突检测
    return name1 == name2;
}

void NamespaceManager::resolveConflict(const std::string& name1, const std::string& name2) {
    // 简化的冲突解决
    // 在实际实现中，这里应该有更复杂的冲突解决逻辑
}

} // namespace CHTL