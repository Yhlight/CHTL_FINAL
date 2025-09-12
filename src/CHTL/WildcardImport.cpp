#include "CHTL/WildcardImport.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace CHTL {

// WildcardImportParser 实现
WildcardImportParser::WildcardImportParser() = default;

WildcardImportParser::~WildcardImportParser() = default;

WildcardPattern WildcardImportParser::parsePattern(const std::string& pattern) const {
    WildcardPattern result;
    result.original_pattern = pattern;
    result.type = detectPatternType(pattern);
    result.base_path = extractBasePath(pattern);
    result.file_extension = extractFileExtension(pattern);
    result.module_name = extractModuleName(pattern);
    result.submodule_name = extractSubmoduleName(pattern);
    
    return result;
}

std::vector<WildcardPattern> WildcardImportParser::parseMultiplePatterns(const std::vector<std::string>& patterns) const {
    std::vector<WildcardPattern> results;
    for (const auto& pattern : patterns) {
        results.push_back(parsePattern(pattern));
    }
    return results;
}

bool WildcardImportParser::isValidPattern(const std::string& pattern) const {
    if (pattern.empty()) {
        return false;
    }
    
    // 检查是否是有效的通配符模式
    return isWildcardPattern(pattern) || isSubmodulePattern(pattern);
}

std::vector<std::string> WildcardImportParser::getPatternValidationErrors(const std::string& pattern) const {
    std::vector<std::string> errors;
    
    if (pattern.empty()) {
        errors.push_back("Pattern cannot be empty");
    }
    
    if (!isValidPattern(pattern)) {
        errors.push_back("Invalid pattern format");
    }
    
    return errors;
}

WildcardImportType WildcardImportParser::detectPatternType(const std::string& pattern) const {
    if (matchesAllFiles(pattern)) {
        return WildcardImportType::ALL_FILES;
    } else if (matchesCMODFiles(pattern)) {
        return WildcardImportType::CMOD_FILES;
    } else if (matchesCJMODFiles(pattern)) {
        return WildcardImportType::CJMOD_FILES;
    } else if (matchesCHTLFiles(pattern)) {
        return WildcardImportType::CHTL_FILES;
    } else if (matchesCSSFiles(pattern)) {
        return WildcardImportType::CSS_FILES;
    } else if (matchesJSFiles(pattern)) {
        return WildcardImportType::JS_FILES;
    } else if (matchesSubmodules(pattern)) {
        return WildcardImportType::SUBMODULES;
    } else if (matchesSpecificSubmodule(pattern)) {
        return WildcardImportType::SPECIFIC_SUBMODULE;
    }
    
    return WildcardImportType::ALL_FILES;
}

bool WildcardImportParser::isWildcardPattern(const std::string& pattern) const {
    return pattern.find('*') != std::string::npos || pattern.find(".*") != std::string::npos;
}

bool WildcardImportParser::isSubmodulePattern(const std::string& pattern) const {
    return pattern.find('.') != std::string::npos && pattern.find('/') == std::string::npos;
}

std::string WildcardImportParser::extractBasePath(const std::string& pattern) const {
    size_t lastSlash = pattern.find_last_of('/');
    size_t lastDot = pattern.find_last_of('.');
    
    if (lastSlash != std::string::npos) {
        return pattern.substr(0, lastSlash);
    } else if (lastDot != std::string::npos && pattern.substr(lastDot) == ".*") {
        return pattern.substr(0, lastDot);
    }
    
    return "";
}

std::string WildcardImportParser::extractFileExtension(const std::string& pattern) const {
    if (pattern.find("*.cmod") != std::string::npos) {
        return ".cmod";
    } else if (pattern.find("*.cjmod") != std::string::npos) {
        return ".cjmod";
    } else if (pattern.find("*.chtl") != std::string::npos) {
        return ".chtl";
    } else if (pattern.find("*.css") != std::string::npos) {
        return ".css";
    } else if (pattern.find("*.js") != std::string::npos) {
        return ".js";
    }
    
    return "";
}

std::string WildcardImportParser::extractModuleName(const std::string& pattern) const {
    if (isSubmodulePattern(pattern)) {
        size_t dotPos = pattern.find('.');
        if (dotPos != std::string::npos) {
            return pattern.substr(0, dotPos);
        }
    }
    
    return "";
}

std::string WildcardImportParser::extractSubmoduleName(const std::string& pattern) const {
    if (isSubmodulePattern(pattern)) {
        size_t dotPos = pattern.find('.');
        if (dotPos != std::string::npos && dotPos + 1 < pattern.length()) {
            std::string sub = pattern.substr(dotPos + 1);
            if (sub != "*") {
                return sub;
            }
        }
    }
    
    return "";
}

bool WildcardImportParser::matchesAllFiles(const std::string& pattern) const {
    return pattern.find(".*") != std::string::npos || pattern.find("/*") != std::string::npos;
}

bool WildcardImportParser::matchesCMODFiles(const std::string& pattern) const {
    return pattern.find("*.cmod") != std::string::npos;
}

bool WildcardImportParser::matchesCJMODFiles(const std::string& pattern) const {
    return pattern.find("*.cjmod") != std::string::npos;
}

bool WildcardImportParser::matchesCHTLFiles(const std::string& pattern) const {
    return pattern.find("*.chtl") != std::string::npos;
}

bool WildcardImportParser::matchesCSSFiles(const std::string& pattern) const {
    return pattern.find("*.css") != std::string::npos;
}

bool WildcardImportParser::matchesJSFiles(const std::string& pattern) const {
    return pattern.find("*.js") != std::string::npos;
}

bool WildcardImportParser::matchesSubmodules(const std::string& pattern) const {
    return pattern.find(".*") != std::string::npos && pattern.find('/') == std::string::npos;
}

bool WildcardImportParser::matchesSpecificSubmodule(const std::string& pattern) const {
    return pattern.find('.') != std::string::npos && pattern.find('*') == std::string::npos && pattern.find('/') == std::string::npos;
}

// WildcardImportResolver 实现
WildcardImportResolver::WildcardImportResolver() {
    // 添加默认搜索路径
    search_paths_.push_back(".");
    search_paths_.push_back("./modules");
    search_paths_.push_back("./src");
}

WildcardImportResolver::~WildcardImportResolver() = default;

std::vector<ImportResult> WildcardImportResolver::resolvePattern(const WildcardPattern& pattern) const {
    std::vector<ImportResult> results;
    
    switch (pattern.type) {
        case WildcardImportType::ALL_FILES: {
            auto files = findAllFiles(pattern.base_path);
            for (const auto& file : files) {
                results.push_back(createImportResult(file, pattern.type));
            }
            break;
        }
        case WildcardImportType::CMOD_FILES: {
            auto files = findCMODFiles(pattern.base_path);
            for (const auto& file : files) {
                results.push_back(createImportResult(file, pattern.type));
            }
            break;
        }
        case WildcardImportType::CJMOD_FILES: {
            auto files = findCJMODFiles(pattern.base_path);
            for (const auto& file : files) {
                results.push_back(createImportResult(file, pattern.type));
            }
            break;
        }
        case WildcardImportType::CHTL_FILES: {
            auto files = findCHTLFiles(pattern.base_path);
            for (const auto& file : files) {
                results.push_back(createImportResult(file, pattern.type));
            }
            break;
        }
        case WildcardImportType::CSS_FILES: {
            auto files = findCSSFiles(pattern.base_path);
            for (const auto& file : files) {
                results.push_back(createImportResult(file, pattern.type));
            }
            break;
        }
        case WildcardImportType::JS_FILES: {
            auto files = findJSFiles(pattern.base_path);
            for (const auto& file : files) {
                results.push_back(createImportResult(file, pattern.type));
            }
            break;
        }
        case WildcardImportType::SUBMODULES: {
            auto submodules = findSubmodules(pattern.module_name);
            for (const auto& submodule : submodules) {
                results.push_back(createImportResult(submodule, pattern.type));
            }
            break;
        }
        case WildcardImportType::SPECIFIC_SUBMODULE: {
            auto submodules = findSpecificSubmodule(pattern.module_name, pattern.submodule_name);
            for (const auto& submodule : submodules) {
                results.push_back(createImportResult(submodule, pattern.type));
            }
            break;
        }
    }
    
    return results;
}

std::vector<ImportResult> WildcardImportResolver::resolvePatterns(const std::vector<WildcardPattern>& patterns) const {
    std::vector<ImportResult> allResults;
    
    for (const auto& pattern : patterns) {
        auto results = resolvePattern(pattern);
        allResults.insert(allResults.end(), results.begin(), results.end());
    }
    
    return allResults;
}

std::vector<std::string> WildcardImportResolver::findFiles(const std::string& basePath, const std::string& extension) const {
    std::vector<std::string> files;
    
    for (const auto& searchPath : search_paths_) {
        std::string fullPath = basePath.empty() ? searchPath : searchPath + "/" + basePath;
        
        if (directoryExists(fullPath)) {
            auto dirFiles = listDirectoryRecursive(fullPath);
            for (const auto& file : dirFiles) {
                if (hasExtension(file, extension)) {
                    files.push_back(file);
                }
            }
        }
    }
    
    return files;
}

std::vector<std::string> WildcardImportResolver::findAllFiles(const std::string& basePath) const {
    std::vector<std::string> files;
    
    auto cmodFiles = findCMODFiles(basePath);
    auto chtlFiles = findCHTLFiles(basePath);
    
    files.insert(files.end(), cmodFiles.begin(), cmodFiles.end());
    files.insert(files.end(), chtlFiles.begin(), chtlFiles.end());
    
    return files;
}

std::vector<std::string> WildcardImportResolver::findCMODFiles(const std::string& basePath) const {
    return findFiles(basePath, ".cmod");
}

std::vector<std::string> WildcardImportResolver::findCJMODFiles(const std::string& basePath) const {
    return findFiles(basePath, ".cjmod");
}

std::vector<std::string> WildcardImportResolver::findCHTLFiles(const std::string& basePath) const {
    return findFiles(basePath, ".chtl");
}

std::vector<std::string> WildcardImportResolver::findCSSFiles(const std::string& basePath) const {
    return findFiles(basePath, ".css");
}

std::vector<std::string> WildcardImportResolver::findJSFiles(const std::string& basePath) const {
    return findFiles(basePath, ".js");
}

std::vector<std::string> WildcardImportResolver::findSubmodules(const std::string& moduleName) const {
    std::vector<std::string> submodules;
    
    // 简化实现：查找模块目录下的所有子模块
    for (const auto& searchPath : search_paths_) {
        std::string modulePath = searchPath + "/" + moduleName;
        if (directoryExists(modulePath)) {
            auto subDirs = listDirectory(modulePath);
            for (const auto& subDir : subDirs) {
                if (directoryExists(subDir)) {
                    submodules.push_back(subDir);
                }
            }
        }
    }
    
    return submodules;
}

std::vector<std::string> WildcardImportResolver::findSpecificSubmodule(const std::string& moduleName, const std::string& submoduleName) const {
    std::vector<std::string> submodules;
    
    for (const auto& searchPath : search_paths_) {
        std::string submodulePath = searchPath + "/" + moduleName + "/" + submoduleName;
        if (directoryExists(submodulePath)) {
            submodules.push_back(submodulePath);
        }
    }
    
    return submodules;
}

std::string WildcardImportResolver::resolvePath(const std::string& path) const {
    if (fileExists(path)) {
        return path;
    }
    
    for (const auto& searchPath : search_paths_) {
        std::string fullPath = searchPath + "/" + path;
        if (fileExists(fullPath)) {
            return fullPath;
        }
    }
    
    return path;
}

std::vector<std::string> WildcardImportResolver::getSearchPaths() const {
    return search_paths_;
}

void WildcardImportResolver::addSearchPath(const std::string& path) {
    if (std::find(search_paths_.begin(), search_paths_.end(), path) == search_paths_.end()) {
        search_paths_.push_back(path);
    }
}

void WildcardImportResolver::removeSearchPath(const std::string& path) {
    search_paths_.erase(std::remove(search_paths_.begin(), search_paths_.end(), path), search_paths_.end());
}

void WildcardImportResolver::clearSearchPaths() {
    search_paths_.clear();
}

std::string WildcardImportResolver::readFileContent(const std::string& filePath) const {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

ImportResult WildcardImportResolver::createImportResult(const std::string& filePath, WildcardImportType type) const {
    ImportResult result;
    result.file_path = filePath;
    result.type = type;
    result.module_name = getFileName(filePath);
    
    if (isValidFile(filePath)) {
        result.content = readFileContent(filePath);
        result.success = !result.content.empty();
        if (!result.success) {
            result.error_message = "Failed to read file content";
        }
    } else {
        result.success = false;
        result.error_message = "Invalid file path";
    }
    
    return result;
}

bool WildcardImportResolver::isValidFile(const std::string& filePath) const {
    return fileExists(filePath);
}

bool WildcardImportResolver::hasExtension(const std::string& filePath, const std::string& extension) const {
    return getFileExtension(filePath) == extension;
}

std::string WildcardImportResolver::getFileExtension(const std::string& filePath) const {
    size_t dotPos = filePath.find_last_of('.');
    if (dotPos != std::string::npos) {
        return filePath.substr(dotPos);
    }
    return "";
}

std::string WildcardImportResolver::getFileName(const std::string& filePath) const {
    size_t slashPos = filePath.find_last_of('/');
    if (slashPos != std::string::npos) {
        return filePath.substr(slashPos + 1);
    }
    return filePath;
}

std::string WildcardImportResolver::getFileDirectory(const std::string& filePath) const {
    size_t slashPos = filePath.find_last_of('/');
    if (slashPos != std::string::npos) {
        return filePath.substr(0, slashPos);
    }
    return "";
}

bool WildcardImportResolver::fileExists(const std::string& filePath) const {
    try {
        return std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath);
    } catch (...) {
        return false;
    }
}

bool WildcardImportResolver::directoryExists(const std::string& dirPath) const {
    try {
        return std::filesystem::exists(dirPath) && std::filesystem::is_directory(dirPath);
    } catch (...) {
        return false;
    }
}

std::vector<std::string> WildcardImportResolver::listDirectory(const std::string& dirPath) const {
    std::vector<std::string> files;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
            files.push_back(entry.path().string());
        }
    } catch (...) {
        // 忽略错误
    }
    
    return files;
}

std::vector<std::string> WildcardImportResolver::listDirectoryRecursive(const std::string& dirPath) const {
    std::vector<std::string> files;
    
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(dirPath)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().string());
            }
        }
    } catch (...) {
        // 忽略错误
    }
    
    return files;
}

// WildcardImportManager 实现
WildcardImportManager::WildcardImportManager() : cache_enabled_(true) {
    parser_ = std::make_shared<WildcardImportParser>();
    resolver_ = std::make_shared<WildcardImportResolver>();
}

WildcardImportManager::~WildcardImportManager() = default;

std::vector<ImportResult> WildcardImportManager::importPattern(const std::string& pattern) {
    // 检查缓存
    if (cache_enabled_ && isCached(pattern)) {
        return getCachedResult(pattern);
    }
    
    // 解析模式
    auto wildcardPattern = parser_->parsePattern(pattern);
    
    // 解析导入
    auto results = resolver_->resolvePattern(wildcardPattern);
    
    // 更新历史和统计
    import_history_.insert(import_history_.end(), results.begin(), results.end());
    updateStatistics(results);
    
    // 缓存结果
    if (cache_enabled_) {
        cacheResult(pattern, results);
    }
    
    return results;
}

std::vector<ImportResult> WildcardImportManager::importPatterns(const std::vector<std::string>& patterns) {
    std::vector<ImportResult> allResults;
    
    for (const auto& pattern : patterns) {
        auto results = importPattern(pattern);
        allResults.insert(allResults.end(), results.begin(), results.end());
    }
    
    return allResults;
}

void WildcardImportManager::enableCache(bool enabled) {
    cache_enabled_ = enabled;
    if (!enabled) {
        clearCache();
    }
}

void WildcardImportManager::clearCache() {
    import_cache_.clear();
}

bool WildcardImportManager::isCacheEnabled() const {
    return cache_enabled_;
}

std::vector<ImportResult> WildcardImportManager::getImportHistory() const {
    return import_history_;
}

void WildcardImportManager::clearImportHistory() {
    import_history_.clear();
}

void WildcardImportManager::setParser(std::shared_ptr<WildcardImportParser> parser) {
    parser_ = parser;
}

void WildcardImportManager::setResolver(std::shared_ptr<WildcardImportResolver> resolver) {
    resolver_ = resolver;
}

std::shared_ptr<WildcardImportParser> WildcardImportManager::getParser() const {
    return parser_;
}

std::shared_ptr<WildcardImportResolver> WildcardImportManager::getResolver() const {
    return resolver_;
}

size_t WildcardImportManager::getTotalImportsCount() const {
    return import_history_.size();
}

size_t WildcardImportManager::getSuccessfulImportsCount() const {
    return std::count_if(import_history_.begin(), import_history_.end(),
                        [](const ImportResult& result) { return result.success; });
}

size_t WildcardImportManager::getFailedImportsCount() const {
    return getTotalImportsCount() - getSuccessfulImportsCount();
}

std::map<WildcardImportType, size_t> WildcardImportManager::getImportStatsByType() const {
    std::map<WildcardImportType, size_t> stats;
    
    for (const auto& result : import_history_) {
        stats[result.type]++;
    }
    
    return stats;
}

bool WildcardImportManager::isCached(const std::string& pattern) const {
    return import_cache_.find(pattern) != import_cache_.end();
}

std::vector<ImportResult> WildcardImportManager::getCachedResult(const std::string& pattern) const {
    auto it = import_cache_.find(pattern);
    if (it != import_cache_.end()) {
        return it->second;
    }
    return {};
}

void WildcardImportManager::cacheResult(const std::string& pattern, const std::vector<ImportResult>& results) {
    import_cache_[pattern] = results;
}

void WildcardImportManager::updateStatistics(const std::vector<ImportResult>& results) {
    // 统计信息在getImportStatsByType等方法中动态计算
}

// SubmoduleImporter 实现
SubmoduleImporter::SubmoduleImporter() : module_base_path_("./modules"), submodule_separator_(".") {
}

SubmoduleImporter::~SubmoduleImporter() = default;

std::vector<ImportResult> SubmoduleImporter::importAllSubmodules(const std::string& moduleName) {
    std::vector<ImportResult> results;
    auto submodules = findAllSubmodules(moduleName);
    
    for (const auto& submodule : submodules) {
        ImportResult result;
        result.file_path = submodule;
        result.module_name = moduleName;
        result.type = WildcardImportType::SUBMODULES;
        result.success = true;
        results.push_back(result);
    }
    
    return results;
}

std::vector<ImportResult> SubmoduleImporter::importSpecificSubmodule(const std::string& moduleName, const std::string& submoduleName) {
    std::vector<ImportResult> results;
    
    if (isValidSubmodule(moduleName, submoduleName)) {
        ImportResult result;
        result.file_path = getSubmodulePath(moduleName, submoduleName);
        result.module_name = moduleName;
        result.type = WildcardImportType::SPECIFIC_SUBMODULE;
        result.success = true;
        results.push_back(result);
    }
    
    return results;
}

std::vector<std::string> SubmoduleImporter::findAllSubmodules(const std::string& moduleName) const {
    std::vector<std::string> submodules;
    
    std::string modulePath = module_base_path_ + "/" + moduleName;
    
    try {
        if (std::filesystem::exists(modulePath) && std::filesystem::is_directory(modulePath)) {
            for (const auto& entry : std::filesystem::directory_iterator(modulePath)) {
                if (entry.is_directory()) {
                    submodules.push_back(entry.path().filename().string());
                }
            }
        }
    } catch (...) {
        // 忽略错误
    }
    
    return submodules;
}

std::vector<std::string> SubmoduleImporter::findSubmodulesByPattern(const std::string& moduleName, const std::string& pattern) const {
    auto allSubmodules = findAllSubmodules(moduleName);
    std::vector<std::string> matchingSubmodules;
    
    std::regex patternRegex(pattern);
    for (const auto& submodule : allSubmodules) {
        if (std::regex_match(submodule, patternRegex)) {
            matchingSubmodules.push_back(submodule);
        }
    }
    
    return matchingSubmodules;
}

bool SubmoduleImporter::isValidSubmodule(const std::string& moduleName, const std::string& submoduleName) const {
    std::string submodulePath = getSubmodulePath(moduleName, submoduleName);
    
    try {
        return std::filesystem::exists(submodulePath) && std::filesystem::is_directory(submodulePath);
    } catch (...) {
        return false;
    }
}

std::vector<std::string> SubmoduleImporter::getSubmoduleValidationErrors(const std::string& moduleName, const std::string& submoduleName) const {
    std::vector<std::string> errors;
    
    if (moduleName.empty()) {
        errors.push_back("Module name cannot be empty");
    }
    
    if (submoduleName.empty()) {
        errors.push_back("Submodule name cannot be empty");
    }
    
    if (!isValidSubmodule(moduleName, submoduleName)) {
        errors.push_back("Submodule does not exist");
    }
    
    return errors;
}

std::string SubmoduleImporter::getSubmodulePath(const std::string& moduleName, const std::string& submoduleName) const {
    return buildSubmodulePath(moduleName, submoduleName);
}

std::string SubmoduleImporter::getSubmoduleDescription(const std::string& moduleName, const std::string& submoduleName) const {
    return "Submodule " + submoduleName + " of module " + moduleName;
}

std::vector<std::string> SubmoduleImporter::getSubmoduleDependencies(const std::string& moduleName, const std::string& submoduleName) const {
    // 简化实现，返回空依赖列表
    return {};
}

void SubmoduleImporter::setModuleBasePath(const std::string& basePath) {
    module_base_path_ = basePath;
}

std::string SubmoduleImporter::getModuleBasePath() const {
    return module_base_path_;
}

void SubmoduleImporter::setSubmoduleSeparator(const std::string& separator) {
    submodule_separator_ = separator;
}

std::string SubmoduleImporter::getSubmoduleSeparator() const {
    return submodule_separator_;
}

std::string SubmoduleImporter::buildSubmodulePath(const std::string& moduleName, const std::string& submoduleName) const {
    return module_base_path_ + "/" + moduleName + "/" + submoduleName;
}

std::vector<std::string> SubmoduleImporter::parseSubmodulePath(const std::string& path) const {
    std::vector<std::string> parts;
    std::stringstream ss(path);
    std::string part;
    
    while (std::getline(ss, part, submodule_separator_[0])) {
        if (!part.empty()) {
            parts.push_back(part);
        }
    }
    
    return parts;
}

bool SubmoduleImporter::isOfficialModule(const std::string& moduleName) const {
    return moduleName == "Chtholly" || moduleName == "Yuigahama";
}

std::string SubmoduleImporter::getOfficialModulePath(const std::string& moduleName) const {
    if (isOfficialModule(moduleName)) {
        return "chtl::" + moduleName;
    }
    return "";
}

// PathEquivalenceSystem 实现
PathEquivalenceSystem::PathEquivalenceSystem() = default;

PathEquivalenceSystem::~PathEquivalenceSystem() = default;

std::string PathEquivalenceSystem::normalizePath(const std::string& path) const {
    return cleanPath(resolveDotSegments(path));
}

bool PathEquivalenceSystem::arePathsEquivalent(const std::string& path1, const std::string& path2) const {
    std::string normalized1 = normalizePath(path1);
    std::string normalized2 = normalizePath(path2);
    
    return normalized1 == normalized2;
}

std::string PathEquivalenceSystem::convertDotToSlash(const std::string& path) const {
    std::string result = path;
    std::replace(result.begin(), result.end(), '.', '/');
    return result;
}

std::string PathEquivalenceSystem::convertSlashToDot(const std::string& path) const {
    std::string result = path;
    std::replace(result.begin(), result.end(), '/', '.');
    return result;
}

bool PathEquivalenceSystem::isValidPath(const std::string& path) const {
    if (path.empty()) {
        return false;
    }
    
    // 检查是否包含无效字符
    std::string invalidChars = "<>:\"|?*";
    for (char c : invalidChars) {
        if (path.find(c) != std::string::npos) {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> PathEquivalenceSystem::getPathValidationErrors(const std::string& path) const {
    std::vector<std::string> errors;
    
    if (path.empty()) {
        errors.push_back("Path cannot be empty");
    }
    
    if (!isValidPath(path)) {
        errors.push_back("Path contains invalid characters");
    }
    
    return errors;
}

std::vector<std::string> PathEquivalenceSystem::splitPath(const std::string& path) const {
    std::vector<std::string> parts;
    std::stringstream ss(path);
    std::string part;
    
    while (std::getline(ss, part, '/')) {
        if (!part.empty() && part != ".") {
            parts.push_back(part);
        }
    }
    
    return parts;
}

std::string PathEquivalenceSystem::joinPath(const std::vector<std::string>& parts) const {
    if (parts.empty()) {
        return "";
    }
    
    std::string result = parts[0];
    for (size_t i = 1; i < parts.size(); ++i) {
        result += "/" + parts[i];
    }
    
    return result;
}

std::string PathEquivalenceSystem::getPathRoot(const std::string& path) const {
    auto parts = splitPath(path);
    return parts.empty() ? "" : parts[0];
}

std::string PathEquivalenceSystem::getPathExtension(const std::string& path) const {
    size_t dotPos = path.find_last_of('.');
    if (dotPos != std::string::npos) {
        return path.substr(dotPos);
    }
    return "";
}

std::string PathEquivalenceSystem::cleanPath(const std::string& path) const {
    std::string result = path;
    
    // 移除双斜杠
    while (result.find("//") != std::string::npos) {
        result.replace(result.find("//"), 2, "/");
    }
    
    // 移除尾部斜杠
    if (result.length() > 1 && result.back() == '/') {
        result.pop_back();
    }
    
    return result;
}

std::string PathEquivalenceSystem::resolveDotSegments(const std::string& path) const {
    auto parts = splitPath(path);
    std::vector<std::string> resolved;
    
    for (const auto& part : parts) {
        if (part == "..") {
            if (!resolved.empty()) {
                resolved.pop_back();
            }
        } else if (part != ".") {
            resolved.push_back(part);
        }
    }
    
    return joinPath(resolved);
}

bool PathEquivalenceSystem::isAbsolutePath(const std::string& path) const {
    return !path.empty() && path[0] == '/';
}

bool PathEquivalenceSystem::isRelativePath(const std::string& path) const {
    return !isAbsolutePath(path);
}

} // namespace CHTL