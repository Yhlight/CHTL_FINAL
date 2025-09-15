#ifndef CMOD_SYSTEM_H
#define CMOD_SYSTEM_H

#include "../CHTLParser/CHTLASTNode.h"
#include "../CHTLContext/CHTLContext.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <fstream>

namespace CHTL {

enum class CMODType {
    CHTL_MODULE,        // CHTL模块
    CJMOD_MODULE,       // CJMOD模块
    MIXED_MODULE,       // 混合模块
    UNKNOWN             // 未知类型
};

struct CMODInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::string mainFile;
    std::vector<std::string> dependencies;
    std::vector<std::string> exports;
    std::vector<std::string> imports;
    CMODType type;
    std::string sourcePath;
    std::string buildPath;
    
    CMODInfo() : type(CMODType::UNKNOWN) {}
};

struct CMODDependency {
    std::string name;
    std::string version;
    std::string source;
    bool isOptional;
    std::string description;
    
    CMODDependency() : isOptional(false) {}
};

struct CMODExport {
    std::string name;
    std::string type; // @Style, @Element, @Var, @Html, @JavaScript
    std::string description;
    bool isPublic;
    std::shared_ptr<CHTLASTNode> content;
    
    CMODExport() : isPublic(true) {}
};

struct CMODImport {
    std::string name;
    std::string source;
    std::string alias;
    std::string type;
    bool isResolved;
    
    CMODImport() : isResolved(false) {}
};

class CMODSystem {
private:
    std::unordered_map<std::string, CMODInfo> modules;
    std::unordered_map<std::string, std::shared_ptr<CHTLContext>> moduleContexts;
    std::string modulePath;
    std::string buildPath;
    bool debugMode;
    
    // 内部方法
    bool validateModule(const CMODInfo& moduleInfo) const;
    bool validateDependency(const CMODDependency& dependency) const;
    bool validateExport(const CMODExport& exportItem) const;
    bool validateImport(const CMODImport& importItem) const;
    
    std::string generateModuleId(const std::string& name, const std::string& version) const;
    std::string getModulePath(const std::string& moduleName) const;
    std::string getBuildPath(const std::string& moduleName) const;
    
    bool loadModuleInfo(const std::string& modulePath, CMODInfo& moduleInfo);
    bool saveModuleInfo(const std::string& modulePath, const CMODInfo& moduleInfo);
    bool loadModuleSource(const std::string& modulePath, std::string& source);
    bool saveModuleBuild(const std::string& buildPath, const std::string& content);
    
    std::vector<std::string> parseDependencies(const std::string& source) const;
    std::vector<std::string> parseExports(const std::string& source) const;
    std::vector<std::string> parseImports(const std::string& source) const;
    
    bool resolveDependencies(const CMODInfo& moduleInfo);
    bool buildModule(const CMODInfo& moduleInfo);
    bool packageModule(const CMODInfo& moduleInfo);
    
public:
    CMODSystem();
    CMODSystem(const std::string& modulePath, const std::string& buildPath);
    ~CMODSystem() = default;
    
    // 模块管理
    bool registerModule(const CMODInfo& moduleInfo);
    bool unregisterModule(const std::string& moduleName);
    bool hasModule(const std::string& moduleName) const;
    CMODInfo getModule(const std::string& moduleName) const;
    std::vector<CMODInfo> getAllModules() const;
    std::vector<CMODInfo> getModulesByType(CMODType type) const;
    
    // 模块加载
    bool loadModule(const std::string& moduleName);
    bool loadModule(const std::string& moduleName, const std::string& version);
    bool loadModuleFromPath(const std::string& modulePath);
    bool unloadModule(const std::string& moduleName);
    
    // 模块构建
    bool buildModule(const std::string& moduleName);
    bool buildAllModules();
    bool rebuildModule(const std::string& moduleName);
    
    // 模块打包
    bool packageModule(const std::string& moduleName);
    bool packageAllModules();
    bool unpackModule(const std::string& packagePath);
    
    // 依赖管理
    bool addDependency(const std::string& moduleName, const CMODDependency& dependency);
    bool removeDependency(const std::string& moduleName, const std::string& dependencyName);
    std::vector<CMODDependency> getDependencies(const std::string& moduleName) const;
    bool resolveDependencies(const std::string& moduleName);
    
    // 导出管理
    bool addExport(const std::string& moduleName, const CMODExport& exportItem);
    bool removeExport(const std::string& moduleName, const std::string& exportName);
    std::vector<CMODExport> getExports(const std::string& moduleName) const;
    CMODExport getExport(const std::string& moduleName, const std::string& exportName) const;
    
    // 导入管理
    bool addImport(const std::string& moduleName, const CMODImport& importItem);
    bool removeImport(const std::string& moduleName, const std::string& importName);
    std::vector<CMODImport> getImports(const std::string& moduleName) const;
    bool resolveImports(const std::string& moduleName);
    
    // 上下文管理
    std::shared_ptr<CHTLContext> getModuleContext(const std::string& moduleName) const;
    bool setModuleContext(const std::string& moduleName, std::shared_ptr<CHTLContext> context);
    
    // 路径管理
    void setModulePath(const std::string& path);
    void setBuildPath(const std::string& path);
    std::string getModulePath() const;
    std::string getBuildPath() const;
    
    // 配置
    void setDebugMode(bool debug);
    bool isDebugMode() const;
    
    // 工具方法
    void clear();
    void reset();
    
    // 调试方法
    void printModules() const;
    void printModuleInfo(const std::string& moduleName) const;
    void printDependencies(const std::string& moduleName) const;
    void printExports(const std::string& moduleName) const;
    void printImports(const std::string& moduleName) const;
    void printStatistics() const;
};

} // namespace CHTL

#endif // CMOD_SYSTEM_H