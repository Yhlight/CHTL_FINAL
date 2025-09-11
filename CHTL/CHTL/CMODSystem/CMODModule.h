#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>
#include <any>
#include <functional>

namespace CHTL {

// 前向声明
class CMODModule;

// 模块类型枚举
enum class ModuleType {
    UNKNOWN = 0,
    CHTL,           // CHTL模块
    CSS,            // CSS模块
    JAVASCRIPT,     // JavaScript模块
    HTML,           // HTML模块
    MIXED,          // 混合模块
    TEMPLATE,       // 模板模块
    COMPONENT,      // 组件模块
    LIBRARY,        // 库模块
    PLUGIN          // 插件模块
};

// 模块状态枚举
enum class ModuleStatus {
    UNLOADED = 0,
    LOADING,
    LOADED,
    ERROR,
    DISABLED
};

// 模块依赖信息
struct ModuleDependency {
    std::string name;
    std::string version;
    std::string path;
    bool required;
    bool optional;
    std::map<std::string, std::any> metadata;
    
    ModuleDependency() : required(true), optional(false) {}
    ModuleDependency(const std::string& name, const std::string& version = "", bool required = true)
        : name(name), version(version), required(required), optional(!required) {}
};

// 模块导出项
struct ModuleExport {
    std::string name;
    std::string type;
    std::any value;
    std::string description;
    bool isDefault;
    std::map<std::string, std::any> metadata;
    
    ModuleExport() : isDefault(false) {}
    ModuleExport(const std::string& name, const std::any& value, const std::string& type = "any")
        : name(name), value(value), type(type), isDefault(false) {}
};

// 模块信息
struct ModuleInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::string homepage;
    std::vector<std::string> keywords;
    std::vector<ModuleDependency> dependencies;
    std::vector<ModuleExport> exports;
    std::map<std::string, std::any> metadata;
    std::string mainFile;
    std::string entryPoint;
    ModuleType type;
    ModuleStatus status;
    
    ModuleInfo() : type(ModuleType::UNKNOWN), status(ModuleStatus::UNLOADED) {}
};

// 模块加载器接口
class IModuleLoader {
public:
    virtual ~IModuleLoader() = default;
    virtual bool canLoad(const std::string& path) = 0;
    virtual std::shared_ptr<CMODModule> load(const std::string& path) = 0;
    virtual std::vector<std::string> getSupportedExtensions() = 0;
};

// CMOD模块类
class CMODModule {
public:
    CMODModule();
    CMODModule(const std::string& name, const std::string& version = "1.0.0");
    CMODModule(const ModuleInfo& info);
    ~CMODModule();
    
    // 基本信息
    void setName(const std::string& name);
    std::string getName() const;
    
    void setVersion(const std::string& version);
    std::string getVersion() const;
    
    void setDescription(const std::string& description);
    std::string getDescription() const;
    
    void setAuthor(const std::string& author);
    std::string getAuthor() const;
    
    void setLicense(const std::string& license);
    std::string getLicense() const;
    
    void setHomepage(const std::string& homepage);
    std::string getHomepage() const;
    
    // 模块信息
    void setInfo(const ModuleInfo& info);
    ModuleInfo getInfo() const;
    
    void setType(ModuleType type);
    ModuleType getType() const;
    
    void setStatus(ModuleStatus status);
    ModuleStatus getStatus() const;
    
    void setMainFile(const std::string& mainFile);
    std::string getMainFile() const;
    
    void setEntryPoint(const std::string& entryPoint);
    std::string getEntryPoint() const;
    
    // 关键词管理
    void addKeyword(const std::string& keyword);
    void removeKeyword(const std::string& keyword);
    void clearKeywords();
    std::vector<std::string> getKeywords() const;
    bool hasKeyword(const std::string& keyword) const;
    
    // 依赖管理
    void addDependency(const ModuleDependency& dependency);
    void addDependency(const std::string& name, const std::string& version = "", bool required = true);
    void removeDependency(const std::string& name);
    void clearDependencies();
    std::vector<ModuleDependency> getDependencies() const;
    std::vector<ModuleDependency> getRequiredDependencies() const;
    std::vector<ModuleDependency> getOptionalDependencies() const;
    bool hasDependency(const std::string& name) const;
    ModuleDependency getDependency(const std::string& name) const;
    
    // 导出管理
    void addExport(const ModuleExport& export_);
    void addExport(const std::string& name, const std::any& value, const std::string& type = "any");
    void addDefaultExport(const std::any& value, const std::string& type = "any");
    void removeExport(const std::string& name);
    void clearExports();
    std::vector<ModuleExport> getExports() const;
    std::vector<ModuleExport> getNamedExports() const;
    ModuleExport getDefaultExport() const;
    bool hasExport(const std::string& name) const;
    ModuleExport getExport(const std::string& name) const;
    std::any getExportValue(const std::string& name) const;
    
    // 元数据管理
    void setMetadata(const std::map<std::string, std::any>& metadata);
    std::map<std::string, std::any> getMetadata() const;
    
    void addMetadata(const std::string& key, const std::any& value);
    std::any getMetadata(const std::string& key) const;
    bool hasMetadata(const std::string& key) const;
    void removeMetadata(const std::string& key);
    void clearMetadata();
    
    // 子模块管理
    void addSubmodule(std::shared_ptr<CMODModule> submodule);
    void removeSubmodule(const std::string& name);
    void clearSubmodules();
    std::vector<std::shared_ptr<CMODModule>> getSubmodules() const;
    std::shared_ptr<CMODModule> getSubmodule(const std::string& name) const;
    bool hasSubmodule(const std::string& name) const;
    
    // 路径管理
    void setPath(const std::string& path);
    std::string getPath() const;
    
    void addSearchPath(const std::string& path);
    void removeSearchPath(const std::string& path);
    void clearSearchPaths();
    std::vector<std::string> getSearchPaths() const;
    
    // 加载状态
    void setLoaded(bool loaded);
    bool isLoaded() const;
    
    void setEnabled(bool enabled);
    bool isEnabled() const;
    
    void setError(const std::string& error);
    std::string getError() const;
    bool hasError() const;
    void clearError();
    
    // 验证
    bool isValid() const;
    bool isComplete() const;
    std::vector<std::string> validate() const;
    
    // 比较
    bool equals(std::shared_ptr<CMODModule> other) const;
    bool equals(const CMODModule& other) const;
    
    // 克隆
    std::shared_ptr<CMODModule> clone() const;
    std::shared_ptr<CMODModule> deepClone() const;
    
    // 转换
    std::string toJSON() const;
    std::string toXML() const;
    std::string toYAML() const;
    std::string toString() const;
    std::string toDebugString() const;
    
    // 格式化
    std::string format() const;
    std::string minify() const;
    std::string beautify() const;
    
    // 统计
    size_t getDependencyCount() const;
    size_t getExportCount() const;
    size_t getSubmoduleCount() const;
    size_t getMetadataCount() const;
    
    // 重置
    void reset();
    
private:
    ModuleInfo info;
    std::string path;
    std::vector<std::string> searchPaths;
    std::vector<std::shared_ptr<CMODModule>> submodules;
    std::string error;
    bool loaded;
    bool enabled;
    
    // 辅助方法
    bool validateModule() const;
    bool validateInfo() const;
    bool validateDependencies() const;
    bool validateExports() const;
    bool validateSubmodules() const;
    bool validateMetadata() const;
    
    bool compareModule(std::shared_ptr<CMODModule> other) const;
    bool compareModule(const CMODModule& other) const;
    bool compareInfo(std::shared_ptr<CMODModule> other) const;
    bool compareInfo(const CMODModule& other) const;
    bool compareDependencies(std::shared_ptr<CMODModule> other) const;
    bool compareDependencies(const CMODModule& other) const;
    bool compareExports(std::shared_ptr<CMODModule> other) const;
    bool compareExports(const CMODModule& other) const;
    bool compareSubmodules(std::shared_ptr<CMODModule> other) const;
    bool compareSubmodules(const CMODModule& other) const;
    bool compareMetadata(std::shared_ptr<CMODModule> other) const;
    bool compareMetadata(const CMODModule& other) const;
    
    void copyTo(std::shared_ptr<CMODModule> target) const;
    void deepCopyTo(std::shared_ptr<CMODModule> target) const;
    
    std::string generateJSON() const;
    std::string generateXML() const;
    std::string generateYAML() const;
    
    std::string formatModule() const;
    std::string minifyModule() const;
    std::string beautifyModule() const;
};

} // namespace CHTL