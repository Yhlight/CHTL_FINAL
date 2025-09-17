#ifndef CHTL_LOADER_H
#define CHTL_LOADER_H

#include "../CHTLContext/CHTLContext.h"
#include "../CHTL/CHTLLexer/CHTLLexer.h"
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../CHTL/CHTLGenerator/CHTLGenerator.h"
#include "../Util/FileSystem/FileSystem.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <set>
#include <functional>

namespace CHTL {

/**
 * CHTL文件加载器
 * 处理文件导入、模块加载、依赖解析等
 */
class CHTLLoader {
public:
    // 导入类型
    enum class ImportType {
        HTML,           // @Html
        STYLE,          // @Style
        JAVASCRIPT,     // @JavaScript
        CHTL,           // @Chtl
        CJMOD,          // @CJmod
        UNKNOWN
    };
    
    // 模块类型
    enum class ModuleType {
        CMOD,           // CHTL模块
        CJMOD,          // CHTL JS模块
        DIRECT,         // 直接文件
        UNKNOWN
    };
    
    // 依赖信息
    struct DependencyInfo {
        std::string name;
        std::string path;
        ImportType type;
        ModuleType moduleType;
        std::string alias;
        std::string namespaceName;
        int line;
        int column;
        bool isLoaded;
        std::string content;
        
        DependencyInfo(const std::string& n, const std::string& p, ImportType t, 
                      ModuleType mt = ModuleType::UNKNOWN, int l = 0, int c = 0)
            : name(n), path(p), type(t), moduleType(mt), line(l), column(c), isLoaded(false) {}
    };
    
    // 模块信息
    struct ModuleInfo {
        std::string name;
        std::string path;
        ModuleType type;
        std::string version;
        std::string description;
        std::string author;
        std::string license;
        std::vector<std::string> dependencies;
        std::map<std::string, std::string> exports;
        std::string content;
        bool isLoaded;
        time_t lastModified;
        
        ModuleInfo(const std::string& n, const std::string& p, ModuleType t)
            : name(n), path(p), type(t), isLoaded(false), lastModified(0) {}
    };
    
    // 文件信息
    struct FileInfo {
        std::string path;
        std::string content;
        time_t lastModified;
        std::vector<DependencyInfo> dependencies;
        bool isParsed;
        std::shared_ptr<CHTLBaseNode> ast;
        
        FileInfo(const std::string& p) : path(p), lastModified(0), isParsed(false) {}
    };
    
    CHTLLoader();
    ~CHTLLoader();
    
    // 主要加载方法
    std::string loadFile(const std::string& filePath);
    std::string loadModule(const std::string& moduleName, const std::string& version = "");
    std::string loadImport(const std::string& importPath, ImportType type, 
                          const std::string& alias = "", const std::string& namespaceName = "");
    
    // 文件管理
    bool addFile(const std::string& filePath);
    bool removeFile(const std::string& filePath);
    bool reloadFile(const std::string& filePath);
    bool isFileLoaded(const std::string& filePath) const;
    FileInfo* getFileInfo(const std::string& filePath);
    
    // 模块管理
    bool addModule(const std::string& modulePath);
    bool removeModule(const std::string& moduleName);
    bool reloadModule(const std::string& moduleName);
    bool isModuleLoaded(const std::string& moduleName) const;
    ModuleInfo* getModuleInfo(const std::string& moduleName);
    std::vector<ModuleInfo> getAllModules() const;
    
    // 依赖解析
    std::vector<DependencyInfo> resolveDependencies(const std::string& filePath);
    std::vector<DependencyInfo> resolveModuleDependencies(const std::string& moduleName);
    bool checkCircularDependencies(const std::string& filePath);
    std::vector<std::string> getDependencyChain(const std::string& filePath);
    
    // 路径解析
    std::string resolvePath(const std::string& path, const std::string& basePath = "");
    std::string findModulePath(const std::string& moduleName, const std::string& version = "");
    std::string findFile(const std::string& fileName, const std::string& basePath = "");
    
    // 搜索路径管理
    void addSearchPath(const std::string& path);
    void removeSearchPath(const std::string& path);
    void clearSearchPaths();
    std::vector<std::string> getSearchPaths() const;
    
    // 模块路径管理
    void setModulePath(const std::string& path);
    std::string getModulePath() const;
    void setOfficialModulePath(const std::string& path);
    std::string getOfficialModulePath() const;
    void setUserModulePath(const std::string& path);
    std::string getUserModulePath() const;
    
    // 缓存管理
    void enableCache(bool enable);
    bool isCacheEnabled() const;
    void clearCache();
    void clearFileCache(const std::string& filePath);
    void clearModuleCache(const std::string& moduleName);
    
    // 错误处理
    void setErrorHandler(std::function<void(const std::string&, int, int)> handler);
    void reportError(const std::string& message, int line = 0, int column = 0);
    bool hasErrors() const;
    const std::vector<std::string>& getErrors() const;
    void clearErrors();
    
    // 统计信息
    size_t getLoadedFileCount() const;
    size_t getLoadedModuleCount() const;
    size_t getCacheSize() const;
    std::string getCacheInfo() const;
    
    // 清理
    void clear();

private:
    // 上下文和编译器
    std::shared_ptr<CHTLContext> context_;
    std::unique_ptr<CHTLLexer> lexer_;
    std::unique_ptr<CHTLParser> parser_;
    std::unique_ptr<CHTLGenerator> generator_;
    
    // 文件管理
    std::map<std::string, FileInfo> loaded_files_;
    std::map<std::string, ModuleInfo> loaded_modules_;
    
    // 搜索路径
    std::vector<std::string> search_paths_;
    std::string module_path_;
    std::string official_module_path_;
    std::string user_module_path_;
    
    // 缓存设置
    bool cache_enabled_;
    std::map<std::string, time_t> file_timestamps_;
    std::map<std::string, time_t> module_timestamps_;
    
    // 错误处理
    std::function<void(const std::string&, int, int)> error_handler_;
    std::vector<std::string> errors_;
    
    // 辅助方法
    std::string loadFileContent(const std::string& filePath);
    std::string loadModuleContent(const std::string& modulePath);
    std::string parseModuleInfo(const std::string& content);
    std::vector<DependencyInfo> extractDependencies(const std::string& content);
    
    // 路径解析辅助
    std::string normalizePath(const std::string& path);
    std::string joinPath(const std::string& base, const std::string& relative);
    bool isAbsolutePath(const std::string& path);
    std::string getDirectory(const std::string& path);
    
    // 模块解析辅助
    ModuleType detectModuleType(const std::string& path);
    ImportType detectImportType(const std::string& path);
    std::string findModuleInPath(const std::string& moduleName, const std::string& searchPath);
    
    // 依赖解析辅助
    bool hasCircularDependency(const std::string& filePath, std::set<std::string>& visited, 
                              std::set<std::string>& recursionStack);
    void buildDependencyChain(const std::string& filePath, std::vector<std::string>& chain, 
                             std::set<std::string>& visited);
    
    // 缓存辅助
    bool isFileModified(const std::string& filePath);
    bool isModuleModified(const std::string& modulePath);
    void updateFileTimestamp(const std::string& filePath);
    void updateModuleTimestamp(const std::string& modulePath);
    
    // 错误处理辅助
    void logError(const std::string& message, int line = 0, int column = 0);
    void validatePath(const std::string& path);
    void validateModule(const std::string& moduleName);
    
    // 初始化
    void initializeDefaultPaths();
    void initializeSearchPaths();
};

} // namespace CHTL

#endif // CHTL_LOADER_H