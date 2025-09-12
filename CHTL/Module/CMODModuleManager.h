#ifndef CMODMODULEMANAGER_H
#define CMODMODULEMANAGER_H

#include "CMODModule.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <mutex>
#include <queue>

namespace CHTL {
namespace Module {

// CMOD模块管理器
class CMODModuleManager {
public:
    // 单例模式
    static CMODModuleManager& getInstance();
    
    // 禁用拷贝构造和赋值
    CMODModuleManager(const CMODModuleManager&) = delete;
    CMODModuleManager& operator=(const CMODModuleManager&) = delete;

    // 模块注册和注销
    bool registerModule(std::shared_ptr<CMODModule> module);
    bool unregisterModule(const std::string& name);
    bool isModuleRegistered(const std::string& name) const;
    
    // 模块获取
    std::shared_ptr<CMODModule> getModule(const std::string& name);
    std::shared_ptr<const CMODModule> getModule(const std::string& name) const;
    std::vector<std::shared_ptr<CMODModule>> getAllModules() const;
    std::vector<std::shared_ptr<CMODModule>> getModulesByType(CMODModuleType type) const;
    std::vector<std::shared_ptr<CMODModule>> getLoadedModules() const;
    
    // 模块加载和卸载
    bool loadModule(const std::string& name);
    bool unloadModule(const std::string& name);
    bool reloadModule(const std::string& name);
    bool enableModule(const std::string& name);
    bool disableModule(const std::string& name);
    
    // 批量操作
    bool loadAllModules();
    bool unloadAllModules();
    bool reloadAllModules();
    bool enableAllModules();
    bool disableAllModules();
    
    // 模块发现
    std::vector<std::string> discoverModules(const std::string& directory);
    bool loadModuleFromFile(const std::string& filePath);
    bool loadModulesFromDirectory(const std::string& directory);
    
    // 依赖管理
    std::vector<std::string> resolveDependencies(const std::string& moduleName) const;
    bool checkDependencies(const std::string& moduleName) const;
    std::vector<std::string> getDependencyChain(const std::string& moduleName) const;
    
    // 模块搜索
    std::vector<std::shared_ptr<CMODModule>> searchModules(const std::string& query) const;
    std::vector<std::shared_ptr<CMODModule>> searchModulesByAuthor(const std::string& author) const;
    std::vector<std::shared_ptr<CMODModule>> searchModulesByLicense(const std::string& license) const;
    std::vector<std::shared_ptr<CMODModule>> searchModulesByTag(const std::string& tag) const;
    
    // 模块信息
    std::vector<std::string> getModuleNames() const;
    std::vector<std::string> getLoadedModuleNames() const;
    std::vector<std::string> getErrorModuleNames() const;
    std::vector<std::string> getDisabledModuleNames() const;
    
    // 统计信息
    size_t getModuleCount() const;
    size_t getLoadedModuleCount() const;
    size_t getErrorModuleCount() const;
    size_t getDisabledModuleCount() const;
    std::map<CMODModuleType, size_t> getModuleCountByType() const;
    
    // 模块配置
    void setModuleDirectory(const std::string& directory);
    std::string getModuleDirectory() const;
    void setAutoLoad(bool autoLoad);
    bool isAutoLoad() const;
    void setLoadOrder(const std::vector<std::string>& order);
    std::vector<std::string> getLoadOrder() const;
    
    // 事件处理
    void setModuleLoadCallback(std::function<void(const std::string&)> callback);
    void setModuleUnloadCallback(std::function<void(const std::string&)> callback);
    void setModuleErrorCallback(std::function<void(const std::string&, const std::string&)> callback);
    
    // 验证和诊断
    bool validateAllModules() const;
    std::vector<std::string> getValidationErrors() const;
    std::vector<std::string> getModuleErrors(const std::string& moduleName) const;
    std::vector<std::string> getModuleWarnings(const std::string& moduleName) const;
    
    // 清理和重置
    void clear();
    void reset();
    
    // 错误处理
    std::vector<std::string> getErrors() const;
    std::vector<std::string> getWarnings() const;
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    void clearErrors();
    void clearWarnings();

private:
    CMODModuleManager();
    ~CMODModuleManager();
    
    // 内部状态
    std::map<std::string, std::shared_ptr<CMODModule>> modules;
    std::string moduleDirectory;
    bool autoLoad;
    std::vector<std::string> loadOrder;
    
    // 事件回调
    std::function<void(const std::string&)> moduleLoadCallback;
    std::function<void(const std::string&)> moduleUnloadCallback;
    std::function<void(const std::string&, const std::string&)> moduleErrorCallback;
    
    // 错误和警告
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // 线程安全
    mutable std::mutex mutex;
    
    // 内部方法
    bool loadModuleInternal(const std::string& name);
    bool unloadModuleInternal(const std::string& name);
    std::vector<std::string> getModuleDependencies(const std::string& moduleName) const;
    bool isModuleDependencySatisfied(const std::string& moduleName) const;
    void notifyModuleLoaded(const std::string& moduleName);
    void notifyModuleUnloaded(const std::string& moduleName);
    void notifyModuleError(const std::string& moduleName, const std::string& error);
    
    // 模块发现
    std::vector<std::string> scanDirectory(const std::string& directory) const;
    bool isValidModuleFile(const std::string& filePath) const;
    std::shared_ptr<CMODModule> createModuleFromFile(const std::string& filePath) const;
    
    // 依赖解析
    std::vector<std::string> topologicalSort(const std::map<std::string, std::vector<std::string>>& dependencies) const;
    bool hasCircularDependency(const std::map<std::string, std::vector<std::string>>& dependencies) const;
    
    // 搜索辅助
    bool matchesQuery(const CMODModule& module, const std::string& query) const;
    bool matchesAuthor(const CMODModule& module, const std::string& author) const;
    bool matchesLicense(const CMODModule& module, const std::string& license) const;
    bool matchesTag(const CMODModule& module, const std::string& tag) const;
};

} // namespace Module
} // namespace CHTL

#endif // CMODMODULEMANAGER_H