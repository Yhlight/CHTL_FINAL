#ifndef CJMODMODULE_H
#define CJMODMODULE_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <chrono>
#include <set>

namespace CHTL {
namespace Module {

// CJMOD模块类型
enum class CJMODModuleType {
    CORE,
    FEATURE,
    UTILITY,
    PLUGIN,
    CUSTOM
};

// CJMOD模块状态
enum class CJMODModuleStatus {
    UNLOADED,
    LOADING,
    LOADED,
    ERROR,
    DISABLED
};

// CJMOD模块信息
struct CJMODModuleInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::string repository;
    std::string documentation;
    CJMODModuleType type;
    CJMODModuleStatus status;
    std::vector<std::string> dependencies;
    std::vector<std::string> exports;
    std::map<std::string, std::string> metadata;
    std::string filePath;
    std::string checksum;
    std::chrono::system_clock::time_point lastModified;
};

// CJMOD API类型
enum class CJMODAPIType {
    FUNCTION,
    CLASS,
    INTERFACE,
    ENUM,
    CONSTANT,
    VARIABLE,
    NAMESPACE
};

// CJMOD API项
struct CJMODAPIItem {
    std::string name;
    CJMODAPIType type;
    std::string signature;
    std::string description;
    std::vector<std::string> parameters;
    std::string returnType;
    std::map<std::string, std::string> attributes;
    bool isPublic;
    bool isStatic;
    bool isAsync;
    std::vector<std::string> tags;
};

// CJMOD子模块
struct CJMODSubModule {
    std::string name;
    std::string path;
    CJMODModuleType type;
    std::vector<std::string> exports;
    std::map<std::string, std::string> metadata;
};

// CJMOD扫描结果
struct CJMODScanResult {
    std::vector<CJMODAPIItem> apiItems;
    std::vector<std::string> imports;
    std::vector<std::string> exports;
    std::map<std::string, std::string> metadata;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    bool success;
    std::chrono::system_clock::time_point scanTime;
};

// CJMOD生成配置
struct CJMODGenerateConfig {
    std::string outputPath;
    std::string format; // "javascript", "typescript", "json", "xml"
    bool minify;
    bool includeComments;
    bool includeSourceMaps;
    std::map<std::string, std::string> options;
};

// CJMOD模块
class CJMODModule {
public:
    CJMODModule();
    explicit CJMODModule(const CJMODModuleInfo& info);
    ~CJMODModule();

    // 模块信息管理
    const CJMODModuleInfo& getInfo() const;
    void setInfo(const CJMODModuleInfo& info);
    void updateInfo(const std::map<std::string, std::string>& updates);
    
    // 模块状态管理
    CJMODModuleStatus getStatus() const;
    void setStatus(CJMODModuleStatus status);
    bool isLoaded() const;
    bool isError() const;
    bool isDisabled() const;
    
    // API管理
    void addAPIItem(const CJMODAPIItem& apiItem);
    void removeAPIItem(const std::string& name);
    CJMODAPIItem* getAPIItem(const std::string& name);
    const CJMODAPIItem* getAPIItem(const std::string& name) const;
    std::vector<CJMODAPIItem> getAllAPIItems() const;
    std::vector<CJMODAPIItem> getPublicAPIItems() const;
    std::vector<CJMODAPIItem> getAPIItemsByType(CJMODAPIType type) const;
    bool hasAPIItem(const std::string& name) const;
    
    // 子模块管理
    void addSubModule(const CJMODSubModule& subModule);
    void removeSubModule(const std::string& name);
    CJMODSubModule* getSubModule(const std::string& name);
    const CJMODSubModule* getSubModule(const std::string& name) const;
    std::vector<CJMODSubModule> getAllSubModules() const;
    bool hasSubModule(const std::string& name) const;
    
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
    
    // 扫描功能
    CJMODScanResult scanCode(const std::string& code) const;
    CJMODScanResult scanFile(const std::string& filePath) const;
    CJMODScanResult scanDirectory(const std::string& directory) const;
    
    // 生成功能
    std::string generateCode(const CJMODGenerateConfig& config) const;
    bool generateToFile(const CJMODGenerateConfig& config) const;
    std::string generateJavaScript() const;
    std::string generateTypeScript() const;
    std::string generateJSON() const;
    std::string generateXML() const;
    
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
    CJMODModuleInfo info;
    std::vector<CJMODAPIItem> apiItems;
    std::vector<CJMODSubModule> subModules;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // 内部方法
    bool validateModuleInfo() const;
    bool validateAPIItems() const;
    bool validateSubModules() const;
    bool validateDependencies() const;
    
    std::string generateChecksum() const;
    bool verifyChecksum() const;
    void updateLastModified();
    
    // 版本比较
    int compareVersionNumbers(const std::string& v1, const std::string& v2) const;
    std::vector<int> parseVersion(const std::string& version) const;
    
    // 扫描辅助方法
    std::vector<CJMODAPIItem> extractAPIItems(const std::string& code) const;
    std::vector<std::string> extractImports(const std::string& code) const;
    std::vector<std::string> extractExports(const std::string& code) const;
    std::map<std::string, std::string> extractMetadata(const std::string& code) const;
    
    // 生成辅助方法
    std::string generateAPIItemCode(const CJMODAPIItem& item) const;
    std::string generateModuleHeader() const;
    std::string generateModuleFooter() const;
};

} // namespace Module
} // namespace CHTL

#endif // CJMODMODULE_H