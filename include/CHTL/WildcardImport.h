#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <set>
#include <regex>
#include <filesystem>

namespace CHTL {

/**
 * 通配符导入类型
 */
enum class WildcardImportType {
    ALL_FILES,          // .* 或 /* - 导入所有.cmod和.chtl文件
    CMOD_FILES,         // *.cmod - 导入所有.cmod文件
    CJMOD_FILES,        // *.cjmod - 导入所有.cjmod文件
    CHTL_FILES,         // *.chtl - 导入所有.chtl文件
    CSS_FILES,          // *.css - 导入所有.css文件
    JS_FILES,           // *.js - 导入所有.js文件
    SUBMODULES,         // Chtholly.* - 导入所有子模块
    SPECIFIC_SUBMODULE  // Chtholly.Space - 导入指定子模块
};

/**
 * 通配符模式
 */
struct WildcardPattern {
    std::string original_pattern;
    WildcardImportType type;
    std::string base_path;
    std::string file_extension;
    std::string module_name;
    std::string submodule_name;
    
    WildcardPattern() : type(WildcardImportType::ALL_FILES) {}
};

/**
 * 导入结果
 */
struct ImportResult {
    std::string file_path;
    std::string module_name;
    std::string content;
    WildcardImportType type;
    bool success;
    std::string error_message;
    
    ImportResult() : success(false) {}
};

/**
 * 通配符导入解析器
 */
class WildcardImportParser {
public:
    WildcardImportParser();
    ~WildcardImportParser();
    
    // 解析通配符模式
    WildcardPattern parsePattern(const std::string& pattern) const;
    std::vector<WildcardPattern> parseMultiplePatterns(const std::vector<std::string>& patterns) const;
    
    // 模式验证
    bool isValidPattern(const std::string& pattern) const;
    std::vector<std::string> getPatternValidationErrors(const std::string& pattern) const;
    
    // 模式类型检测
    WildcardImportType detectPatternType(const std::string& pattern) const;
    bool isWildcardPattern(const std::string& pattern) const;
    bool isSubmodulePattern(const std::string& pattern) const;
    
private:
    // 解析辅助函数
    std::string extractBasePath(const std::string& pattern) const;
    std::string extractFileExtension(const std::string& pattern) const;
    std::string extractModuleName(const std::string& pattern) const;
    std::string extractSubmoduleName(const std::string& pattern) const;
    
    // 模式匹配
    bool matchesAllFiles(const std::string& pattern) const;
    bool matchesCMODFiles(const std::string& pattern) const;
    bool matchesCJMODFiles(const std::string& pattern) const;
    bool matchesCHTLFiles(const std::string& pattern) const;
    bool matchesCSSFiles(const std::string& pattern) const;
    bool matchesJSFiles(const std::string& pattern) const;
    bool matchesSubmodules(const std::string& pattern) const;
    bool matchesSpecificSubmodule(const std::string& pattern) const;
};

/**
 * 通配符导入解析器
 */
class WildcardImportResolver {
public:
    WildcardImportResolver();
    ~WildcardImportResolver();
    
    // 解析导入
    std::vector<ImportResult> resolvePattern(const WildcardPattern& pattern) const;
    std::vector<ImportResult> resolvePatterns(const std::vector<WildcardPattern>& patterns) const;
    
    // 文件查找
    std::vector<std::string> findFiles(const std::string& basePath, const std::string& extension) const;
    std::vector<std::string> findAllFiles(const std::string& basePath) const;
    std::vector<std::string> findCMODFiles(const std::string& basePath) const;
    std::vector<std::string> findCJMODFiles(const std::string& basePath) const;
    std::vector<std::string> findCHTLFiles(const std::string& basePath) const;
    std::vector<std::string> findCSSFiles(const std::string& basePath) const;
    std::vector<std::string> findJSFiles(const std::string& basePath) const;
    
    // 子模块查找
    std::vector<std::string> findSubmodules(const std::string& moduleName) const;
    std::vector<std::string> findSpecificSubmodule(const std::string& moduleName, const std::string& submoduleName) const;
    
    // 路径解析
    std::string resolvePath(const std::string& path) const;
    std::vector<std::string> getSearchPaths() const;
    void addSearchPath(const std::string& path);
    void removeSearchPath(const std::string& path);
    void clearSearchPaths();
    
    // 内容读取
    std::string readFileContent(const std::string& filePath) const;
    
private:
    std::vector<std::string> search_paths_;
    
    // 解析辅助函数
    ImportResult createImportResult(const std::string& filePath, WildcardImportType type) const;
    bool isValidFile(const std::string& filePath) const;
    bool hasExtension(const std::string& filePath, const std::string& extension) const;
    std::string getFileExtension(const std::string& filePath) const;
    std::string getFileName(const std::string& filePath) const;
    std::string getFileDirectory(const std::string& filePath) const;
    
    // 文件系统操作
    bool fileExists(const std::string& filePath) const;
    bool directoryExists(const std::string& dirPath) const;
    std::vector<std::string> listDirectory(const std::string& dirPath) const;
    std::vector<std::string> listDirectoryRecursive(const std::string& dirPath) const;
};

/**
 * 通配符导入管理器
 */
class WildcardImportManager {
public:
    WildcardImportManager();
    ~WildcardImportManager();
    
    // 导入管理
    std::vector<ImportResult> importPattern(const std::string& pattern);
    std::vector<ImportResult> importPatterns(const std::vector<std::string>& patterns);
    
    // 缓存管理
    void enableCache(bool enabled);
    void clearCache();
    bool isCacheEnabled() const;
    
    // 导入历史
    std::vector<ImportResult> getImportHistory() const;
    void clearImportHistory();
    
    // 配置
    void setParser(std::shared_ptr<WildcardImportParser> parser);
    void setResolver(std::shared_ptr<WildcardImportResolver> resolver);
    
    std::shared_ptr<WildcardImportParser> getParser() const;
    std::shared_ptr<WildcardImportResolver> getResolver() const;
    
    // 统计信息
    size_t getTotalImportsCount() const;
    size_t getSuccessfulImportsCount() const;
    size_t getFailedImportsCount() const;
    std::map<WildcardImportType, size_t> getImportStatsByType() const;
    
private:
    std::shared_ptr<WildcardImportParser> parser_;
    std::shared_ptr<WildcardImportResolver> resolver_;
    std::vector<ImportResult> import_history_;
    std::map<std::string, std::vector<ImportResult>> import_cache_;
    bool cache_enabled_;
    
    // 缓存操作
    bool isCached(const std::string& pattern) const;
    std::vector<ImportResult> getCachedResult(const std::string& pattern) const;
    void cacheResult(const std::string& pattern, const std::vector<ImportResult>& results);
    
    // 统计辅助函数
    void updateStatistics(const std::vector<ImportResult>& results);
};

/**
 * 子模块导入系统
 */
class SubmoduleImporter {
public:
    SubmoduleImporter();
    ~SubmoduleImporter();
    
    // 子模块导入
    std::vector<ImportResult> importAllSubmodules(const std::string& moduleName);
    std::vector<ImportResult> importSpecificSubmodule(const std::string& moduleName, const std::string& submoduleName);
    
    // 子模块查找
    std::vector<std::string> findAllSubmodules(const std::string& moduleName) const;
    std::vector<std::string> findSubmodulesByPattern(const std::string& moduleName, const std::string& pattern) const;
    
    // 子模块验证
    bool isValidSubmodule(const std::string& moduleName, const std::string& submoduleName) const;
    std::vector<std::string> getSubmoduleValidationErrors(const std::string& moduleName, const std::string& submoduleName) const;
    
    // 模块信息
    std::string getSubmodulePath(const std::string& moduleName, const std::string& submoduleName) const;
    std::string getSubmoduleDescription(const std::string& moduleName, const std::string& submoduleName) const;
    std::vector<std::string> getSubmoduleDependencies(const std::string& moduleName, const std::string& submoduleName) const;
    
    // 配置
    void setModuleBasePath(const std::string& basePath);
    std::string getModuleBasePath() const;
    
    void setSubmoduleSeparator(const std::string& separator);
    std::string getSubmoduleSeparator() const;
    
private:
    std::string module_base_path_;
    std::string submodule_separator_;
    
    // 辅助函数
    std::string buildSubmodulePath(const std::string& moduleName, const std::string& submoduleName) const;
    std::vector<std::string> parseSubmodulePath(const std::string& path) const;
    bool isOfficialModule(const std::string& moduleName) const;
    std::string getOfficialModulePath(const std::string& moduleName) const;
};

/**
 * 路径等价系统
 */
class PathEquivalenceSystem {
public:
    PathEquivalenceSystem();
    ~PathEquivalenceSystem();
    
    // 路径等价
    std::string normalizePath(const std::string& path) const;
    bool arePathsEquivalent(const std::string& path1, const std::string& path2) const;
    
    // . / 对等式处理
    std::string convertDotToSlash(const std::string& path) const;
    std::string convertSlashToDot(const std::string& path) const;
    
    // 路径验证
    bool isValidPath(const std::string& path) const;
    std::vector<std::string> getPathValidationErrors(const std::string& path) const;
    
    // 路径解析
    std::vector<std::string> splitPath(const std::string& path) const;
    std::string joinPath(const std::vector<std::string>& parts) const;
    std::string getPathRoot(const std::string& path) const;
    std::string getPathExtension(const std::string& path) const;
    
private:
    // 辅助函数
    std::string cleanPath(const std::string& path) const;
    std::string resolveDotSegments(const std::string& path) const;
    bool isAbsolutePath(const std::string& path) const;
    bool isRelativePath(const std::string& path) const;
};

} // namespace CHTL