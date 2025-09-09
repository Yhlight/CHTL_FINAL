#include "ImportManager.hpp"
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>
#include <filesystem>

namespace CHTL {

ImportManager& ImportManager::getInstance() {
    static ImportManager instance;
    return instance;
}

void ImportManager::addImport(const std::string& alias, ImportType type, 
                             const std::string& path) {
    addImport(alias, type, path, {});
}

void ImportManager::addImport(const std::string& alias, ImportType type, 
                             const std::string& path,
                             const std::map<std::string, std::string>& attributes) {
    auto importDef = std::make_shared<ImportDefinition>();
    importDef->type = type;
    importDef->path = path;
    importDef->alias = alias;
    importDef->attributes = attributes;
    importDef->isResolved = false;
    
    // 尝试解析路径
    std::string resolvedPath = resolveImportPath(path, type);
    if (!resolvedPath.empty()) {
        importDef->path = resolvedPath;
        importDef->isResolved = true;
        importDef->content = loadFileContent(resolvedPath);
    }
    
    imports_[alias] = importDef;
}

std::shared_ptr<ImportManager::ImportDefinition> ImportManager::getImport(const std::string& alias) const {
    auto it = imports_.find(alias);
    return (it != imports_.end()) ? it->second : nullptr;
}

bool ImportManager::hasImport(const std::string& alias) const {
    return imports_.find(alias) != imports_.end();
}

std::string ImportManager::resolveImportPath(const std::string& path, ImportType type) const {
    // 处理绝对路径
    if (std::filesystem::path(path).is_absolute()) {
        return resolveAbsolutePath(path);
    }
    
    // 处理相对路径
    if (path.find("./") == 0 || path.find("../") == 0) {
        return resolveRelativePath(path);
    }
    
    // 处理模块路径
    if (path.find("chtl::") == 0) {
        return resolveModulePath(path, type);
    }
    
    // 处理通配符导入
    if (path.find("*") != std::string::npos) {
        auto wildcardPaths = resolveWildcardImport(path, type);
        if (!wildcardPaths.empty()) {
            return wildcardPaths[0]; // 返回第一个匹配的文件
        }
    }
    
    // 在搜索路径中查找
    return searchFile(path, type);
}

std::string ImportManager::loadImportContent(const std::string& alias) const {
    auto importDef = getImport(alias);
    if (!importDef) {
        return "";
    }
    
    if (importDef->isResolved && !importDef->content.empty()) {
        return importDef->content;
    }
    
    if (!importDef->path.empty()) {
        return loadFileContent(importDef->path);
    }
    
    return "";
}

std::vector<std::string> ImportManager::getAllImportNames() const {
    std::vector<std::string> names;
    for (const auto& [name, importDef] : imports_) {
        names.push_back(name);
    }
    return names;
}

std::vector<std::string> ImportManager::getImportNamesByType(ImportType type) const {
    std::vector<std::string> names;
    for (const auto& [name, importDef] : imports_) {
        if (importDef->type == type) {
            names.push_back(name);
        }
    }
    return names;
}

void ImportManager::clear() {
    imports_.clear();
}

void ImportManager::removeImport(const std::string& alias) {
    imports_.erase(alias);
}

void ImportManager::addSearchPath(const std::string& path) {
    if (std::find(searchPaths_.begin(), searchPaths_.end(), path) == searchPaths_.end()) {
        searchPaths_.push_back(path);
    }
}

void ImportManager::setCurrentDirectory(const std::string& dir) {
    currentDirectory_ = dir;
}

std::vector<std::string> ImportManager::resolveWildcardImport(const std::string& pattern, ImportType type) const {
    std::vector<std::string> results;
    
    // 获取搜索路径
    auto modulePaths = getModuleSearchPaths(type);
    
    for (const auto& searchPath : modulePaths) {
        auto expanded = expandWildcardPattern(pattern, searchPath);
        results.insert(results.end(), expanded.begin(), expanded.end());
    }
    
    return results;
}

bool ImportManager::fileExists(const std::string& path) const {
    return std::filesystem::exists(path);
}

std::string ImportManager::getFileExtension(const std::string& path) const {
    std::filesystem::path filePath(path);
    return filePath.extension().string();
}

bool ImportManager::validateImportType(const std::string& path, ImportType type) const {
    std::string extension = getFileExtension(path);
    
    switch (type) {
        case ImportType::HTML:
            return extension == ".html" || extension == ".htm";
        case ImportType::CSS:
            return extension == ".css";
        case ImportType::JAVASCRIPT:
            return extension == ".js";
        case ImportType::CJJS:
            return extension == ".cjjs";
        case ImportType::CHTL:
            return extension == ".chtl";
        case ImportType::CMOD:
            return extension == ".cmod";
        case ImportType::CJMOD:
            return extension == ".cjmod";
        default:
            return false;
    }
}

std::string ImportManager::searchFile(const std::string& filename, ImportType type) const {
    // 在当前目录中查找
    if (fileExists(filename) && validateImportType(filename, type)) {
        return filename;
    }
    
    // 在搜索路径中查找
    for (const auto& searchPath : searchPaths_) {
        std::string fullPath = searchPath + "/" + filename;
        if (fileExists(fullPath) && validateImportType(fullPath, type)) {
            return fullPath;
        }
    }
    
    // 在模块搜索路径中查找
    auto modulePaths = getModuleSearchPaths(type);
    for (const auto& modulePath : modulePaths) {
        std::string fullPath = modulePath + "/" + filename;
        if (fileExists(fullPath) && validateImportType(fullPath, type)) {
            return fullPath;
        }
    }
    
    return "";
}

std::string ImportManager::loadFileContent(const std::string& path) const {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }
    
    std::ostringstream content;
    content << file.rdbuf();
    return content.str();
}

std::string ImportManager::resolveRelativePath(const std::string& path) const {
    if (currentDirectory_.empty()) {
        return path;
    }
    
    std::filesystem::path currentPath(currentDirectory_);
    std::filesystem::path relativePath(path);
    std::filesystem::path resolvedPath = currentPath / relativePath;
    
    return resolvedPath.string();
}

std::string ImportManager::resolveAbsolutePath(const std::string& path) const {
    if (fileExists(path)) {
        return path;
    }
    return "";
}

std::string ImportManager::resolveModulePath(const std::string& path, ImportType type) const {
    // 移除 chtl:: 前缀
    std::string moduleName = path;
    if (moduleName.find("chtl::") == 0) {
        moduleName = moduleName.substr(6);
    }
    
    // 在模块搜索路径中查找
    auto modulePaths = getModuleSearchPaths(type);
    for (const auto& modulePath : modulePaths) {
        std::string fullPath = modulePath + "/" + moduleName;
        if (fileExists(fullPath) && validateImportType(fullPath, type)) {
            return fullPath;
        }
    }
    
    return "";
}

std::vector<std::string> ImportManager::getModuleSearchPaths(ImportType type) const {
    std::vector<std::string> paths;
    
    // 添加默认模块路径
    paths.push_back("./module");
    paths.push_back("./modules");
    
    // 添加编译器模块路径
    paths.push_back("./chtl/module");
    paths.push_back("./chtl/modules");
    
    // 根据类型添加特定路径
    switch (type) {
        case ImportType::CMOD:
            paths.push_back("./module/cmod");
            paths.push_back("./chtl/module/cmod");
            break;
        case ImportType::CJMOD:
            paths.push_back("./module/cjmod");
            paths.push_back("./chtl/module/cjmod");
            break;
        default:
            break;
    }
    
    return paths;
}

bool ImportManager::validateFileType(const std::string& path, ImportType type) const {
    return validateImportType(path, type);
}

std::vector<std::string> ImportManager::expandWildcardPattern(const std::string& pattern, 
                                                             const std::string& directory) const {
    std::vector<std::string> results;
    
    try {
        std::filesystem::path dirPath(directory);
        if (!std::filesystem::exists(dirPath) || !std::filesystem::is_directory(dirPath)) {
            return results;
        }
        
        // 将通配符模式转换为正则表达式
        std::string regexPattern = pattern;
        std::regex_replace(regexPattern, std::regex("\\*"), ".*");
        std::regex_replace(regexPattern, std::regex("\\?"), ".");
        std::regex patternRegex(regexPattern);
        
        // 遍历目录中的文件
        for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (std::regex_match(filename, patternRegex)) {
                    results.push_back(entry.path().string());
                }
            }
        }
    } catch (const std::exception& e) {
        // 忽略错误，返回空结果
    }
    
    return results;
}

} // namespace CHTL