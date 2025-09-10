#ifndef CHTL_CMOD_SYSTEM_H
#define CHTL_CMOD_SYSTEM_H

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {

class CMODSystem {
private:
    std::string modulePath;
    std::vector<std::string> loadedModules;
    std::map<std::string, std::string> moduleCache;
    std::vector<std::string> errors;
    bool debugMode;

public:
    CMODSystem();
    CMODSystem(const std::string& path);
    virtual ~CMODSystem() = default;
    
    // 基本属性
    const std::string& getModulePath() const { return modulePath; }
    void setModulePath(const std::string& path) { modulePath = path; }
    
    // 模块管理
    bool loadModule(const std::string& moduleName);
    bool unloadModule(const std::string& moduleName);
    bool isModuleLoaded(const std::string& moduleName) const;
    std::vector<std::string> getLoadedModules() const { return loadedModules; }
    
    // 模块缓存
    void cacheModule(const std::string& moduleName, const std::string& content);
    std::string getCachedModule(const std::string& moduleName) const;
    bool isModuleCached(const std::string& moduleName) const;
    void clearCache();
    
    // 错误管理
    const std::vector<std::string>& getErrors() const { return errors; }
    bool hasErrors() const { return !errors.empty(); }
    void clearErrors();
    
    // 模式设置
    bool isDebugMode() const { return debugMode; }
    void setDebugMode(bool mode) { debugMode = mode; }
    
    // 工具方法
    std::string resolveModulePath(const std::string& moduleName) const;
    bool moduleExists(const std::string& moduleName) const;
    std::string getModuleContent(const std::string& moduleName) const;
    
    // 完整API
    bool scanModule(const std::string& moduleName);
    bool generateModule(const std::string& moduleName, const std::string& outputPath);
    bool processModule(const std::string& moduleName);
    bool optimizeModule(const std::string& moduleName);
    bool validateModuleSyntax(const std::string& moduleName) const;
    bool validateModuleSemantics(const std::string& moduleName) const;
    
    // 模块依赖管理
    std::vector<std::string> getModuleDependencies(const std::string& moduleName) const;
    bool resolveDependencies(const std::string& moduleName);
    bool checkCircularDependencies(const std::string& moduleName) const;
    
    // 模块版本管理
    std::string getModuleVersion(const std::string& moduleName) const;
    bool updateModuleVersion(const std::string& moduleName, const std::string& version);
    bool compareModuleVersions(const std::string& module1, const std::string& module2) const;
    
    // 模块配置管理
    std::map<std::string, std::string> getModuleConfig(const std::string& moduleName) const;
    bool setModuleConfig(const std::string& moduleName, const std::map<std::string, std::string>& config);
    bool updateModuleConfig(const std::string& moduleName, const std::string& key, const std::string& value);
    
    // 模块统计信息
    std::map<std::string, int> getModuleStatistics(const std::string& moduleName) const;
    std::string getModuleReport(const std::string& moduleName) const;
    std::string getSystemReport() const;
    
    // 字符串表示
    std::string toString() const;
    std::string toDebugString() const;
    
    // 序列化
    std::string toJSON() const;
    std::string toXML() const;
    std::string toYAML() const;
    
    // 克隆
    CMODSystem clone() const;
    
    // 比较
    bool operator==(const CMODSystem& other) const;
    bool operator!=(const CMODSystem& other) const;
};

} // namespace CHTL

#endif // CHTL_CMOD_SYSTEM_H