#include "OfficialModuleManager.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace CHTL {
namespace Official {

// 静态成员初始化
std::unique_ptr<OfficialModuleManager> OfficialModuleManager::instance = nullptr;
std::map<std::string, std::function<std::shared_ptr<CMODModule>()>> ModuleFactory::moduleCreators;
std::map<std::string, std::string> ModuleConfiguration::globalConfigurations;

// OfficialModuleManager 实现
OfficialModuleManager::OfficialModuleManager() {
    initializeDefaultModules();
    setupModuleDependencies();
}

OfficialModuleManager& OfficialModuleManager::getInstance() {
    if (!instance) {
        instance = std::make_unique<OfficialModuleManager>();
    }
    return *instance;
}

bool OfficialModuleManager::registerModule(std::shared_ptr<CMODModule> module) {
    if (!module || !validateModule(module)) {
        return false;
    }
    
    std::string moduleName = module->getModuleName();
    modules[moduleName] = module;
    moduleStates[moduleName] = false;
    
    return true;
}

bool OfficialModuleManager::unregisterModule(const std::string& moduleName) {
    auto it = modules.find(moduleName);
    if (it == modules.end()) {
        return false;
    }
    
    // 清理模块
    if (moduleStates[moduleName]) {
        it->second->cleanup();
    }
    
    modules.erase(it);
    moduleStates.erase(moduleName);
    
    return true;
}

std::shared_ptr<CMODModule> OfficialModuleManager::getModule(const std::string& moduleName) const {
    auto it = modules.find(moduleName);
    if (it != modules.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::string> OfficialModuleManager::getAvailableModules() const {
    std::vector<std::string> moduleNames;
    for (const auto& pair : modules) {
        moduleNames.push_back(pair.first);
    }
    return moduleNames;
}

bool OfficialModuleManager::initializeAllModules() {
    bool allInitialized = true;
    
    for (auto& pair : modules) {
        if (!initializeModule(pair.first)) {
            allInitialized = false;
        }
    }
    
    return allInitialized;
}

bool OfficialModuleManager::initializeModule(const std::string& moduleName) {
    auto it = modules.find(moduleName);
    if (it == modules.end()) {
        return false;
    }
    
    if (moduleStates[moduleName]) {
        return true; // 已经初始化
    }
    
    // 检查依赖
    if (!checkModuleDependencies(moduleName)) {
        std::cerr << "Module dependencies not satisfied for: " << moduleName << std::endl;
        return false;
    }
    
    // 初始化模块
    if (it->second->initialize()) {
        moduleStates[moduleName] = true;
        return true;
    }
    
    return false;
}

void OfficialModuleManager::cleanupAllModules() {
    for (auto& pair : modules) {
        if (moduleStates[pair.first]) {
            pair.second->cleanup();
            moduleStates[pair.first] = false;
        }
    }
}

void OfficialModuleManager::cleanupModule(const std::string& moduleName) {
    auto it = modules.find(moduleName);
    if (it != modules.end() && moduleStates[moduleName]) {
        it->second->cleanup();
        moduleStates[moduleName] = false;
    }
}

bool OfficialModuleManager::isModuleInitialized(const std::string& moduleName) const {
    auto it = moduleStates.find(moduleName);
    return it != moduleStates.end() && it->second;
}

bool OfficialModuleManager::areAllModulesInitialized() const {
    for (const auto& pair : moduleStates) {
        if (!pair.second) {
            return false;
        }
    }
    return true;
}

std::string OfficialModuleManager::processWithModule(const std::string& moduleName, const std::string& content) const {
    auto module = getModule(moduleName);
    if (!module || !isModuleInitialized(moduleName)) {
        return "";
    }
    
    // 根据模块类型进行不同的处理
    if (moduleName == "Chtholly") {
        auto chthollyModule = std::dynamic_pointer_cast<ChthollyModule>(module);
        if (chthollyModule) {
            return chthollyModule->generateHTML(content);
        }
    } else if (moduleName == "Yuigahama") {
        auto yuigahamaModule = std::dynamic_pointer_cast<YuigahamaModule>(module);
        if (yuigahamaModule) {
            return yuigahamaModule->generateAdvancedHTML(content);
        }
    }
    
    return content;
}

std::string OfficialModuleManager::processWithAllModules(const std::string& content) const {
    std::string result = content;
    
    for (const auto& pair : modules) {
        if (isModuleInitialized(pair.first)) {
            std::string processed = processWithModule(pair.first, result);
            if (!processed.empty()) {
                result = processed;
            }
        }
    }
    
    return result;
}

void OfficialModuleManager::setModuleConfiguration(const std::string& moduleName, const std::string& key, const std::string& value) {
    auto module = getModule(moduleName);
    if (module) {
        // 尝试转换为ChthollyModule或YuigahamaModule
        auto chthollyModule = std::dynamic_pointer_cast<ChthollyModule>(module);
        if (chthollyModule) {
            chthollyModule->setConfiguration(key, value);
            return;
        }
        
        auto yuigahamaModule = std::dynamic_pointer_cast<YuigahamaModule>(module);
        if (yuigahamaModule) {
            yuigahamaModule->setConfiguration(key, value);
            return;
        }
    }
}

std::string OfficialModuleManager::getModuleConfiguration(const std::string& moduleName, const std::string& key) const {
    auto module = getModule(moduleName);
    if (module) {
        // 尝试转换为ChthollyModule或YuigahamaModule
        auto chthollyModule = std::dynamic_pointer_cast<ChthollyModule>(module);
        if (chthollyModule) {
            return chthollyModule->getConfiguration(key);
        }
        
        auto yuigahamaModule = std::dynamic_pointer_cast<YuigahamaModule>(module);
        if (yuigahamaModule) {
            return yuigahamaModule->getConfiguration(key);
        }
    }
    
    return "";
}

std::string OfficialModuleManager::getModuleInfo(const std::string& moduleName) const {
    auto module = getModule(moduleName);
    if (!module) {
        return "";
    }
    
    std::ostringstream info;
    info << "Module: " << module->getModuleName() << "\n";
    info << "Version: " << module->getVersion() << "\n";
    info << "Description: " << module->getDescription() << "\n";
    info << "Author: " << module->getAuthor() << "\n";
    info << "Initialized: " << (isModuleInitialized(moduleName) ? "Yes" : "No") << "\n";
    
    return info.str();
}

std::map<std::string, std::string> OfficialModuleManager::getAllModulesInfo() const {
    std::map<std::string, std::string> allInfo;
    
    for (const auto& pair : modules) {
        allInfo[pair.first] = getModuleInfo(pair.first);
    }
    
    return allInfo;
}

bool OfficialModuleManager::checkModuleDependencies(const std::string& moduleName) const {
    auto it = moduleDependencies.find(moduleName);
    if (it == moduleDependencies.end()) {
        return true; // 没有依赖
    }
    
    for (const auto& dependency : it->second) {
        if (!isModuleInitialized(dependency)) {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> OfficialModuleManager::getModuleDependencies(const std::string& moduleName) const {
    auto it = moduleDependencies.find(moduleName);
    if (it != moduleDependencies.end()) {
        return it->second;
    }
    return {};
}

void OfficialModuleManager::initializeDefaultModules() {
    // 注册默认模块
    registerModule(std::make_shared<ChthollyModule>());
    registerModule(std::make_shared<YuigahamaModule>());
}

void OfficialModuleManager::setupModuleDependencies() {
    // 设置模块依赖关系
    moduleDependencies["Yuigahama"] = {"Chtholly"};
}

bool OfficialModuleManager::validateModule(const std::shared_ptr<CMODModule>& module) const {
    if (!module) {
        return false;
    }
    
    // 检查模块基本信息
    if (module->getModuleName().empty() || 
        module->getVersion().empty() || 
        module->getDescription().empty() || 
        module->getAuthor().empty()) {
        return false;
    }
    
    return true;
}

// ModuleFactory 实现
std::shared_ptr<CMODModule> ModuleFactory::createModule(const std::string& moduleName) {
    initializeModuleCreators();
    
    auto it = moduleCreators.find(moduleName);
    if (it != moduleCreators.end()) {
        return it->second();
    }
    
    return nullptr;
}

std::vector<std::string> ModuleFactory::getSupportedModules() {
    initializeModuleCreators();
    
    std::vector<std::string> supportedModules;
    for (const auto& pair : moduleCreators) {
        supportedModules.push_back(pair.first);
    }
    
    return supportedModules;
}

bool ModuleFactory::isModuleSupported(const std::string& moduleName) {
    initializeModuleCreators();
    
    return moduleCreators.find(moduleName) != moduleCreators.end();
}

void ModuleFactory::initializeModuleCreators() {
    if (!moduleCreators.empty()) {
        return; // 已经初始化
    }
    
    moduleCreators["Chtholly"] = []() { return std::make_shared<ChthollyModule>(); };
    moduleCreators["Yuigahama"] = []() { return std::make_shared<YuigahamaModule>(); };
}

// ModuleConfiguration 实现
void ModuleConfiguration::setGlobalConfiguration(const std::string& key, const std::string& value) {
    globalConfigurations[key] = value;
}

std::string ModuleConfiguration::getGlobalConfiguration(const std::string& key) {
    auto it = globalConfigurations.find(key);
    if (it != globalConfigurations.end()) {
        return it->second;
    }
    return "";
}

std::map<std::string, std::string> ModuleConfiguration::getAllGlobalConfigurations() {
    return globalConfigurations;
}

void ModuleConfiguration::clearGlobalConfigurations() {
    globalConfigurations.clear();
}

} // namespace Official
} // namespace CHTL