#ifndef CMODMODULE_H
#define CMODMODULE_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <chrono>
#include <set>

namespace CHTL {
namespace Module {

// CMOD模块类型
enum class CMODModuleType {
    CORE,
    FEATURE,
    UTILITY,
    PLUGIN,
    CUSTOM
};

// CMOD模块状态
enum class CMODModuleStatus {
    UNLOADED,
    LOADING,
    LOADED,
    ERROR,
    DISABLED
};

// CMOD模块信息
struct CMODModuleInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::string repository;
    std::string documentation;
    CMODModuleType type;
    CMODModuleStatus status;
    std::vector<std::string> dependencies;
    std::vector<std::string> exports;
    std::map<std::string, std::string> metadata;
    std::string filePath;
    std::string checksum;
    std::chrono::system_clock::time_point lastModified;
};

// CMOD导出项
struct CMODExport {
    std::string name;
    std::string type;
    std::string value;
    std::string description;
    bool isPublic;
    std::vector<std::string> parameters;
    std::map<std::string, std::string> attributes;
};

// CMOD子模块
struct CMODSubModule {
    std::string name;
    std::string path;
    CMODModuleType type;
    std::vector<std::string> exports;
    std::map<std::string, std::string> metadata;
};

// CMOD导入路径
struct CMODImportPath {
    std::string moduleName;
    std::string exportName;
    std::string alias;
    std::string path;
    bool isWildcard;
    std::vector<std::string> conditions;
};

// CMOD模块
class CMODModule {
public:
    CMODModule();
    explicit CMODModule(const CMODModuleInfo& info);
    ~CMODModule();

    // 模块信息管理
    const CMODModuleInfo& getInfo() const;
    void setInfo(const CMODModuleInfo& info);
    void updateInfo(const std::map<std::string, std::string>& updates);
    
    // 模块状态管理
    CMODModuleStatus getStatus() const;
    void setStatus(CMODModuleStatus status);
    bool isLoaded() const;
    bool isError() const;
    bool isDisabled() const;
    
    // 导出管理
    void addExport(const CMODExport& exportItem);
    void removeExport(const std::string& name);
    CMODExport* getExport(const std::string& name);
    const CMODExport* getExport(const std::string& name) const;
    std::vector<CMODExport> getAllExports() const;
    std::vector<CMODExport> getPublicExports() const;
    bool hasExport(const std::string& name) const;
    
    // 子模块管理
    void addSubModule(const CMODSubModule& subModule);
    void removeSubModule(const std::string& name);
    CMODSubModule* getSubModule(const std::string& name);
    const CMODSubModule* getSubModule(const std::string& name) const;
    std::vector<CMODSubModule> getAllSubModules() const;
    bool hasSubModule(const std::string& name) const;
    
    // 导入路径管理
    void addImportPath(const CMODImportPath& importPath);
    void removeImportPath(const std::string& moduleName);
    CMODImportPath* getImportPath(const std::string& moduleName);
    const CMODImportPath* getImportPath(const std::string& moduleName) const;
    std::vector<CMODImportPath> getAllImportPaths() const;
    bool hasImportPath(const std::string& moduleName) const;
    
    // 依赖管理
    void addDependency(const std::string& dependency);
    void removeDependency(const std::string& dependency);
    std::vector<std::string> getDependencies() const;
    bool hasDependency(const std::string& dependency) const;
    bool isDependencySatisfied(const std::string& dependency) const;
    
    // 元数据管理
    void setMetadata(const std::string& key, const std::string& value);
    std::string getMetadata(const std::string& key) const;
    bool hasMetadata(const std::string& key) const;
    void removeMetadata(const std::string& key);
    std::map<std::string, std::string> getAllMetadata() const;
    
    // 模块操作
    bool load();
    bool unload();
    bool reload();
    bool enable();
    bool disable();
    
    // 验证
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // 序列化
    std::string toJSON() const;
    bool fromJSON(const std::string& json);
    std::string toXML() const;
    bool fromXML(const std::string& xml);
    
    // 工具方法
    std::string getDisplayName() const;
    std::string getVersionString() const;
    std::string getTypeString() const;
    std::string getStatusString() const;
    bool isCompatible(const std::string& version) const;
    int compareVersion(const std::string& version) const;
    
    // 错误处理
    std::vector<std::string> getErrors() const;
    std::vector<std::string> getWarnings() const;
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    void clearErrors();
    void clearWarnings();

private:
    CMODModuleInfo info;
    std::vector<CMODExport> exports;
    std::vector<CMODSubModule> subModules;
    std::vector<CMODImportPath> importPaths;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // 内部方法
    bool validateModuleInfo() const;
    bool validateExports() const;
    bool validateSubModules() const;
    bool validateImportPaths() const;
    bool validateDependencies() const;
    
    std::string generateChecksum() const;
    bool verifyChecksum() const;
    void updateLastModified();
    
    // 版本比较
    int compareVersionNumbers(const std::string& v1, const std::string& v2) const;
    std::vector<int> parseVersion(const std::string& version) const;
};

} // namespace Module
} // namespace CHTL

#endif // CMODMODULE_H