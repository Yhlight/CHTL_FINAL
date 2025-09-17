#ifndef CMODSYSTEM_H
#define CMODSYSTEM_H

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {

/**
 * CMOD模块信息
 */
struct CMODInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::string dependencies;
    std::string category;
    std::string minCHTLVersion;
    std::string maxCHTLVersion;
};

/**
 * CMOD导出项
 */
struct CMODExport {
    std::string type;  // Custom, Template, Origin, Configuration
    std::string name;
    std::string content;
};

/**
 * CMOD模块
 */
class CMODModule {
public:
    CMODModule();
    ~CMODModule();
    
    // 基本信息
    void setInfo(const CMODInfo& info);
    const CMODInfo& getInfo() const;
    
    // 源码管理
    void addSourceFile(const std::string& filename, const std::string& content);
    const std::map<std::string, std::string>& getSourceFiles() const;
    std::string getSourceFile(const std::string& filename) const;
    
    // 导出管理
    void addExport(const CMODExport& export_item);
    const std::vector<CMODExport>& getExports() const;
    
    // 子模块管理
    void addSubModule(const std::string& name, std::shared_ptr<CMODModule> submodule);
    const std::map<std::string, std::shared_ptr<CMODModule>>& getSubModules() const;
    std::shared_ptr<CMODModule> getSubModule(const std::string& name) const;
    
    // 验证
    bool isValid() const;
    std::vector<std::string> getValidationErrors() const;

private:
    CMODInfo info_;
    std::map<std::string, std::string> source_files_;
    std::vector<CMODExport> exports_;
    std::map<std::string, std::shared_ptr<CMODModule>> sub_modules_;
    
    bool validateInfo() const;
    bool validateSourceFiles() const;
    bool validateExports() const;
};

/**
 * CMOD系统
 */
class CMODSystem {
public:
    CMODSystem();
    ~CMODSystem();
    
    // 模块管理
    bool loadModule(const std::string& path);
    bool loadModuleFromFile(const std::string& filepath);
    bool saveModule(const std::string& name, const std::string& path);
    
    std::shared_ptr<CMODModule> getModule(const std::string& name) const;
    const std::map<std::string, std::shared_ptr<CMODModule>>& getAllModules() const;
    
    // 模块搜索
    std::vector<std::string> searchModules(const std::string& query) const;
    std::vector<std::string> getModulesByCategory(const std::string& category) const;
    
    // 依赖管理
    std::vector<std::string> resolveDependencies(const std::string& moduleName) const;
    bool checkDependencies(const std::string& moduleName) const;
    
    // 模块打包
    bool packModule(const std::string& moduleName, const std::string& outputPath);
    bool unpackModule(const std::string& filepath, const std::string& outputPath);
    
    // 配置
    void setModulePath(const std::string& path);
    const std::string& getModulePath() const;
    
    void setDebugMode(bool debug);
    bool isDebugMode() const;

private:
    std::map<std::string, std::shared_ptr<CMODModule>> modules_;
    std::string module_path_;
    bool debug_mode_;
    
    // 内部方法
    bool parseModuleInfo(const std::string& content, CMODInfo& info);
    bool parseModuleExports(const std::string& content, std::vector<CMODExport>& exports);
    std::string readFile(const std::string& filepath);
    bool writeFile(const std::string& filepath, const std::string& content);
    std::vector<std::string> getFilesInDirectory(const std::string& dirpath);
    bool isDirectory(const std::string& path);
    bool fileExists(const std::string& path);
};

} // namespace CHTL

#endif // CMODSYSTEM_H