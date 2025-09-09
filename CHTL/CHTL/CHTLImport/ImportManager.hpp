#pragma once

#include <string>
#include <map>
#include <memory>
#include <vector>
#include <filesystem>
#include "CHTLNode/BaseNode.hpp"

namespace CHTL {

/**
 * @brief 导入管理器
 * 
 * 管理文件导入和路径搜索策略
 */
class ImportManager {
public:
    enum class ImportType {
        HTML,           // HTML文件
        CSS,            // CSS文件
        JAVASCRIPT,     // JavaScript文件
        CJJS,           // CHTL JS文件
        CHTL,           // CHTL文件
        CMOD,           // CMOD模块
        CJMOD           // CJMOD模块
    };
    
    struct ImportDefinition {
        ImportType type;
        std::string path;
        std::string alias;
        std::map<std::string, std::string> attributes;
        std::string content;
        bool isResolved;
    };
    
    // 单例模式
    static ImportManager& getInstance();
    
    // 添加导入定义
    void addImport(const std::string& alias, ImportType type, 
                  const std::string& path);
    
    // 添加带属性的导入定义
    void addImport(const std::string& alias, ImportType type, 
                  const std::string& path,
                  const std::map<std::string, std::string>& attributes);
    
    // 获取导入定义
    std::shared_ptr<ImportDefinition> getImport(const std::string& alias) const;
    
    // 检查导入是否存在
    bool hasImport(const std::string& alias) const;
    
    // 解析导入路径
    std::string resolveImportPath(const std::string& path, ImportType type) const;
    
    // 加载导入内容
    std::string loadImportContent(const std::string& alias) const;
    
    // 获取所有导入名称
    std::vector<std::string> getAllImportNames() const;
    
    // 获取指定类型的所有导入名称
    std::vector<std::string> getImportNamesByType(ImportType type) const;
    
    // 清除所有导入
    void clear();
    
    // 移除导入
    void removeImport(const std::string& alias);
    
    // 设置搜索路径
    void addSearchPath(const std::string& path);
    
    // 获取搜索路径
    const std::vector<std::string>& getSearchPaths() const { return searchPaths_; }
    
    // 设置当前工作目录
    void setCurrentDirectory(const std::string& dir);
    
    // 获取当前工作目录
    const std::string& getCurrentDirectory() const { return currentDirectory_; }
    
    // 解析通配符导入
    std::vector<std::string> resolveWildcardImport(const std::string& pattern, ImportType type) const;
    
    // 检查文件是否存在
    bool fileExists(const std::string& path) const;
    
    // 获取文件扩展名
    std::string getFileExtension(const std::string& path) const;
    
    // 验证导入类型
    bool validateImportType(const std::string& path, ImportType type) const;
    
private:
    ImportManager() = default;
    ~ImportManager() = default;
    ImportManager(const ImportManager&) = delete;
    ImportManager& operator=(const ImportManager&) = delete;
    
    std::map<std::string, std::shared_ptr<ImportDefinition>> imports_;
    std::vector<std::string> searchPaths_;
    std::string currentDirectory_;
    
    // 搜索文件
    std::string searchFile(const std::string& filename, ImportType type) const;
    
    // 加载文件内容
    std::string loadFileContent(const std::string& path) const;
    
    // 处理相对路径
    std::string resolveRelativePath(const std::string& path) const;
    
    // 处理绝对路径
    std::string resolveAbsolutePath(const std::string& path) const;
    
    // 处理模块路径
    std::string resolveModulePath(const std::string& path, ImportType type) const;
    
    // 获取模块搜索路径
    std::vector<std::string> getModuleSearchPaths(ImportType type) const;
    
    // 验证文件类型
    bool validateFileType(const std::string& path, ImportType type) const;
    
    // 处理通配符模式
    std::vector<std::string> expandWildcardPattern(const std::string& pattern, 
                                                   const std::string& directory) const;
};

} // namespace CHTL