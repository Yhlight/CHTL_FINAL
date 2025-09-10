#ifndef CHTL_OFFICIAL_MODULE_MANAGER_H
#define CHTL_OFFICIAL_MODULE_MANAGER_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include "ChthollyModule/ChthollyModule.h"
#include "YuigahamaModule/YuigahamaModule.h"

namespace CHTL {
namespace Official {

// 官方模块管理器
class OfficialModuleManager {
public:
    OfficialModuleManager();
    ~OfficialModuleManager() = default;
    
    // 单例模式
    static OfficialModuleManager& getInstance();
    
    // 模块管理
    bool registerModule(std::shared_ptr<CMODModule> module);
    bool unregisterModule(const std::string& moduleName);
    std::shared_ptr<CMODModule> getModule(const std::string& moduleName) const;
    std::vector<std::string> getAvailableModules() const;
    
    // 模块初始化
    bool initializeAllModules();
    bool initializeModule(const std::string& moduleName);
    void cleanupAllModules();
    void cleanupModule(const std::string& moduleName);
    
    // 模块状态
    bool isModuleInitialized(const std::string& moduleName) const;
    bool areAllModulesInitialized() const;
    
    // 模块功能
    std::string processWithModule(const std::string& moduleName, const std::string& content) const;
    std::string processWithAllModules(const std::string& content) const;
    
    // 配置管理
    void setModuleConfiguration(const std::string& moduleName, const std::string& key, const std::string& value);
    std::string getModuleConfiguration(const std::string& moduleName, const std::string& key) const;
    
    // 模块信息
    std::string getModuleInfo(const std::string& moduleName) const;
    std::map<std::string, std::string> getAllModulesInfo() const;
    
    // 模块依赖
    bool checkModuleDependencies(const std::string& moduleName) const;
    std::vector<std::string> getModuleDependencies(const std::string& moduleName) const;
    
private:
    static std::unique_ptr<OfficialModuleManager> instance;
    std::map<std::string, std::shared_ptr<CMODModule>> modules;
    std::map<std::string, bool> moduleStates;
    std::map<std::string, std::vector<std::string>> moduleDependencies;
    
    void initializeDefaultModules();
    void setupModuleDependencies();
    bool validateModule(const std::shared_ptr<CMODModule>& module) const;
};

// 模块工厂
class ModuleFactory {
public:
    static std::shared_ptr<CMODModule> createModule(const std::string& moduleName);
    static std::vector<std::string> getSupportedModules();
    static bool isModuleSupported(const std::string& moduleName);
    
private:
    static std::map<std::string, std::function<std::shared_ptr<CMODModule>()>> moduleCreators;
    static void initializeModuleCreators();
};

// 模块配置
class ModuleConfiguration {
public:
    static void setGlobalConfiguration(const std::string& key, const std::string& value);
    static std::string getGlobalConfiguration(const std::string& key);
    static std::map<std::string, std::string> getAllGlobalConfigurations();
    static void clearGlobalConfigurations();
    
private:
    static std::map<std::string, std::string> globalConfigurations;
};

} // namespace Official
} // namespace CHTL

#endif // CHTL_OFFICIAL_MODULE_MANAGER_H