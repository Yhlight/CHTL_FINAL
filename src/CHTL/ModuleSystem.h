#ifndef MODULE_SYSTEM_H
#define MODULE_SYSTEM_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

namespace CHTL {

// 模块类型
enum class ModuleType {
    CMOD,       // CHTL模块
    CJMOD,      // CHTL JS模块
    MIXED,      // 混合模块
    UNKNOWN
};

// 模块信息
struct ModuleInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::vector<std::string> dependencies;
    std::vector<std::string> keywords;
    std::string repository;
    std::string homepage;
    
    ModuleInfo() = default;
    ModuleInfo(const std::string& n, const std::string& v)
        : name(n), version(v) {}
};

// 导出项
struct ExportItem {
    std::string name;
    std::string type;
    std::string description;
    bool isPublic;
    
    ExportItem() : isPublic(false) {}
    ExportItem(const std::string& n, const std::string& t, bool pub = false)
        : name(n), type(t), isPublic(pub) {}
};

// 导出表
class ExportTable {
public:
    ExportTable() = default;
    ~ExportTable() = default;
    
    // 导出项管理
    void addExport(const ExportItem& item);
    void addExport(const std::string& name, const std::string& type, bool isPublic = false);
    void removeExport(const std::string& name);
    
    // 查询导出项
    ExportItem getExport(const std::string& name) const;
    bool hasExport(const std::string& name) const;
    std::vector<ExportItem> getAllExports() const;
    std::vector<ExportItem> getPublicExports() const;
    
    // 清理
    void clear();
    
private:
    std::unordered_map<std::string, ExportItem> exports_;
};

// 模块定义
class Module {
public:
    Module(ModuleType type, const std::string& name);
    ~Module() = default;
    
    // 基本信息
    ModuleType getType() const { return type_; }
    const std::string& getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }
    
    // 模块信息
    void setInfo(const ModuleInfo& info);
    const ModuleInfo& getInfo() const { return info_; }
    
    // 导出表管理
    void setExportTable(std::shared_ptr<ExportTable> exportTable);
    std::shared_ptr<ExportTable> getExportTable() const { return exportTable_; }
    
    // 内容管理
    void setContent(const std::string& content);
    const std::string& getContent() const { return content_; }
    
    // 路径管理
    void setPath(const std::string& path);
    const std::string& getPath() const { return path_; }
    
    // 依赖管理
    void addDependency(const std::string& dependency);
    const std::vector<std::string>& getDependencies() const { return dependencies_; }
    bool hasDependency(const std::string& dependency) const;
    
    // 验证
    bool isValid() const;
    
private:
    ModuleType type_;
    std::string name_;
    ModuleInfo info_;
    std::shared_ptr<ExportTable> exportTable_;
    std::string content_;
    std::string path_;
    std::vector<std::string> dependencies_;
};

// 模块系统
class ModuleSystem {
public:
    ModuleSystem();
    ~ModuleSystem() = default;
    
    // 模块管理
    void registerModule(std::shared_ptr<Module> module);
    std::shared_ptr<Module> getModule(const std::string& name) const;
    bool hasModule(const std::string& name) const;
    void removeModule(const std::string& name);
    
    // 模块加载
    std::shared_ptr<Module> loadModule(const std::string& path);
    std::shared_ptr<Module> loadCMOD(const std::string& path);
    std::shared_ptr<Module> loadCJMOD(const std::string& path);
    std::shared_ptr<Module> loadMixedModule(const std::string& path);
    
    // 模块解析
    std::shared_ptr<Module> parseModule(const std::string& content, const std::string& path);
    ModuleInfo parseModuleInfo(const std::string& content);
    std::shared_ptr<ExportTable> parseExportTable(const std::string& content);
    
    // 模块搜索
    std::vector<std::string> searchModules(const std::string& query) const;
    std::vector<std::string> searchModulesByType(ModuleType type) const;
    std::vector<std::string> searchModulesByKeyword(const std::string& keyword) const;
    
    // 依赖解析
    std::vector<std::string> resolveDependencies(const std::string& moduleName) const;
    bool checkDependencies(const std::string& moduleName) const;
    
    // 模块路径
    void addModulePath(const std::string& path);
    const std::vector<std::string>& getModulePaths() const { return modulePaths_; }
    std::string findModule(const std::string& name) const;
    
    // 官方模块
    void addOfficialModule(const std::string& name, const std::string& path);
    std::string getOfficialModule(const std::string& name) const;
    bool isOfficialModule(const std::string& name) const;
    
    // 错误处理
    void setErrorHandler(std::function<void(const std::string&, size_t, size_t)> handler);
    
    // 清理
    void clear();
    
private:
    std::unordered_map<std::string, std::shared_ptr<Module>> modules_;
    std::vector<std::string> modulePaths_;
    std::unordered_map<std::string, std::string> officialModules_;
    std::function<void(const std::string&, size_t, size_t)> errorHandler_;
    
    // 辅助方法
    void reportError(const std::string& message, size_t line = 0, size_t column = 0);
    
    // 模块解析辅助方法
    std::string extractModuleInfo(const std::string& content) const;
    std::string extractExportTable(const std::string& content) const;
    std::string extractModuleContent(const std::string& content) const;
    
    // 路径处理
    std::string resolveModulePath(const std::string& name) const;
    std::string getModuleDirectory(const std::string& path) const;
    std::string getModuleFileName(const std::string& path) const;
    
    // 文件操作
    bool fileExists(const std::string& path) const;
    std::string readFile(const std::string& path) const;
    bool writeFile(const std::string& path, const std::string& content) const;
    
    // 依赖处理
    void processDependencies(std::shared_ptr<Module> module);
    bool isDependencyResolved(const std::string& dependency) const;
    
    // 模块验证
    bool validateModule(std::shared_ptr<Module> module) const;
    bool validateModuleInfo(const ModuleInfo& info) const;
    bool validateExportTable(std::shared_ptr<ExportTable> exportTable) const;
};

} // namespace CHTL

#endif // MODULE_SYSTEM_H