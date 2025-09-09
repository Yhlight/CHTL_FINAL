#pragma once

#include "CHTLContext.h"
#include "CHTLNode.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace CHTL {

/**
 * @brief 文件类型枚举
 */
enum class FileType {
    CHTL,       // .chtl文件
    HTML,       // .html文件
    CSS,        // .css文件
    JS,         // .js文件
    CJJS,       // .cjjs文件
    CMOD,       // .cmod文件
    CJMOD,      // .cjmod文件
    UNKNOWN     // 未知类型
};

/**
 * @brief 模块信息结构
 */
struct ModuleInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::string dependencies;
    std::string category;
    std::string minCHTLVersion;
    std::string maxCHTLVersion;
    
    std::unordered_map<std::string, std::vector<std::string>> exports;
};

/**
 * @brief CHTL文件加载器
 */
class CHTLLoader {
public:
    CHTLLoader(std::shared_ptr<CHTLContext> context);
    ~CHTLLoader() = default;

    /**
     * @brief 加载文件
     * @param filePath 文件路径
     * @return 加载的AST根节点
     */
    std::shared_ptr<CHTLNode> loadFile(const std::string& filePath);
    
    /**
     * @brief 加载模块
     * @param moduleName 模块名称
     * @return 加载的AST根节点
     */
    std::shared_ptr<CHTLNode> loadModule(const std::string& moduleName);
    
    /**
     * @brief 加载CMOD模块
     * @param modulePath CMOD文件路径
     * @return 加载的AST根节点
     */
    std::shared_ptr<CHTLNode> loadCMOD(const std::string& modulePath);
    
    /**
     * @brief 加载CJMOD模块
     * @param modulePath CJMOD文件路径
     * @return 加载的AST根节点
     */
    std::shared_ptr<CHTLNode> loadCJMOD(const std::string& modulePath);

    /**
     * @brief 获取文件类型
     * @param filePath 文件路径
     * @return 文件类型
     */
    FileType getFileType(const std::string& filePath) const;
    
    /**
     * @brief 检查文件是否存在
     * @param filePath 文件路径
     * @return 是否存在
     */
    bool fileExists(const std::string& filePath) const;
    
    /**
     * @brief 获取模块信息
     * @param modulePath 模块路径
     * @return 模块信息
     */
    ModuleInfo getModuleInfo(const std::string& modulePath) const;

    /**
     * @brief 添加搜索路径
     * @param path 搜索路径
     */
    void addSearchPath(const std::string& path);
    
    /**
     * @brief 设置官方模块路径
     * @param path 官方模块路径
     */
    void setOfficialModulePath(const std::string& path);

    /**
     * @brief 获取加载错误
     * @return 错误信息列表
     */
    const std::vector<std::string>& getErrors() const;
    
    /**
     * @brief 是否有错误
     * @return 是否有错误
     */
    bool hasErrors() const;

private:
    std::shared_ptr<CHTLContext> m_context;
    std::vector<std::string> m_searchPaths;
    std::string m_officialModulePath;
    std::vector<std::string> m_errors;
    
    // 缓存
    std::unordered_map<std::string, std::shared_ptr<CHTLNode>> m_loadedFiles;
    std::unordered_map<std::string, ModuleInfo> m_moduleInfoCache;
    
    // 文件处理
    std::string readFile(const std::string& filePath) const;
    std::string findFile(const std::string& fileName) const;
    std::string findModule(const std::string& moduleName) const;
    
    // 模块处理
    std::shared_ptr<CHTLNode> loadCMODModule(const std::string& modulePath);
    std::shared_ptr<CHTLNode> loadCJMODModule(const std::string& modulePath);
    ModuleInfo parseModuleInfo(const std::string& infoPath) const;
    std::vector<std::string> parseExports(const std::string& exportPath) const;
    
    // 路径处理
    std::string resolvePath(const std::string& path) const;
    std::string getDirectory(const std::string& filePath) const;
    std::string getFileName(const std::string& filePath) const;
    std::string getFileExtension(const std::string& filePath) const;
    
    // 通配符处理
    std::vector<std::string> expandWildcard(const std::string& pattern) const;
    bool matchesPattern(const std::string& fileName, const std::string& pattern) const;
    
    // 错误处理
    void addError(const std::string& error);
    
    // 工具函数
    bool isAbsolutePath(const std::string& path) const;
    std::string normalizePath(const std::string& path) const;
    std::string joinPath(const std::string& base, const std::string& path) const;
};

} // namespace CHTL