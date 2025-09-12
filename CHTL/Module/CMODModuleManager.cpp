#include "CMODModuleManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <set>
#include <queue>

namespace CHTL {
namespace Module {

// 单例实现
CMODModuleManager& CMODModuleManager::getInstance() {
    static CMODModuleManager instance;
    return instance;
}

CMODModuleManager::CMODModuleManager() : autoLoad(true) {
    // 初始化模块管理器
}

CMODModuleManager::~CMODModuleManager() {
    // 清理资源
    clear();
}

// 模块注册和注销实现
bool CMODModuleManager::registerModule(std::shared_ptr<CMODModule> module) {
    if (!module) {
        addError("模块指针为空");
        return false;
    }
    
    std::lock_guard<std::mutex> lock(mutex);
    
    const std::string& name = module->getInfo().name;
    if (name.empty()) {
        addError("模块名称为空");
        return false;
    }
    
    if (modules.find(name) != modules.end()) {
        addWarning("模块已存在: " + name);
        return false;
    }
    
    modules[name] = module;
    
    if (autoLoad) {
        loadModuleInternal(name);
    }
    
    return true;
}

bool CMODModuleManager::unregisterModule(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = modules.find(name);
    if (it == modules.end()) {
        addError("模块不存在: " + name);
        return false;
    }
    
    if (it->second->isLoaded()) {
        unloadModuleInternal(name);
    }
    
    modules.erase(it);
    return true;
}

bool CMODModuleManager::isModuleRegistered(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex);
    return modules.find(name) != modules.end();
}

// 模块获取实现
std::shared_ptr<CMODModule> CMODModuleManager::getModule(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = modules.find(name);
    return (it != modules.end()) ? it->second : nullptr;
}

std::shared_ptr<const CMODModule> CMODModuleManager::getModule(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = modules.find(name);
    return (it != modules.end()) ? it->second : nullptr;
}

std::vector<std::shared_ptr<CMODModule>> CMODModuleManager::getAllModules() const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::shared_ptr<CMODModule>> result;
    for (const auto& pair : modules) {
        result.push_back(pair.second);
    }
    return result;
}

std::vector<std::shared_ptr<CMODModule>> CMODModuleManager::getModulesByType(CMODModuleType type) const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::shared_ptr<CMODModule>> result;
    for (const auto& pair : modules) {
        if (pair.second->getInfo().type == type) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::vector<std::shared_ptr<CMODModule>> CMODModuleManager::getLoadedModules() const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::shared_ptr<CMODModule>> result;
    for (const auto& pair : modules) {
        if (pair.second->isLoaded()) {
            result.push_back(pair.second);
        }
    }
    return result;
}

// 模块加载和卸载实现
bool CMODModuleManager::loadModule(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    return loadModuleInternal(name);
}

bool CMODModuleManager::unloadModule(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    return unloadModuleInternal(name);
}

bool CMODModuleManager::reloadModule(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = modules.find(name);
    if (it == modules.end()) {
        addError("模块不存在: " + name);
        return false;
    }
    
    if (it->second->isLoaded()) {
        if (!unloadModuleInternal(name)) {
            return false;
        }
    }
    
    return loadModuleInternal(name);
}

bool CMODModuleManager::enableModule(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = modules.find(name);
    if (it == modules.end()) {
        addError("模块不存在: " + name);
        return false;
    }
    
    return it->second->enable();
}

bool CMODModuleManager::disableModule(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = modules.find(name);
    if (it == modules.end()) {
        addError("模块不存在: " + name);
        return false;
    }
    
    return it->second->disable();
}

// 批量操作实现
bool CMODModuleManager::loadAllModules() {
    std::lock_guard<std::mutex> lock(mutex);
    
    bool allLoaded = true;
    for (const auto& pair : modules) {
        if (!loadModuleInternal(pair.first)) {
            allLoaded = false;
        }
    }
    
    return allLoaded;
}

bool CMODModuleManager::unloadAllModules() {
    std::lock_guard<std::mutex> lock(mutex);
    
    bool allUnloaded = true;
    for (const auto& pair : modules) {
        if (pair.second->isLoaded()) {
            if (!unloadModuleInternal(pair.first)) {
                allUnloaded = false;
            }
        }
    }
    
    return allUnloaded;
}

bool CMODModuleManager::reloadAllModules() {
    std::lock_guard<std::mutex> lock(mutex);
    
    bool allReloaded = true;
    for (const auto& pair : modules) {
        if (!reloadModule(pair.first)) {
            allReloaded = false;
        }
    }
    
    return allReloaded;
}

bool CMODModuleManager::enableAllModules() {
    std::lock_guard<std::mutex> lock(mutex);
    
    bool allEnabled = true;
    for (const auto& pair : modules) {
        if (!pair.second->enable()) {
            allEnabled = false;
        }
    }
    
    return allEnabled;
}

bool CMODModuleManager::disableAllModules() {
    std::lock_guard<std::mutex> lock(mutex);
    
    bool allDisabled = true;
    for (const auto& pair : modules) {
        if (!pair.second->disable()) {
            allDisabled = false;
        }
    }
    
    return allDisabled;
}

// 模块发现实现
std::vector<std::string> CMODModuleManager::discoverModules(const std::string& directory) {
    return scanDirectory(directory);
}

bool CMODModuleManager::loadModuleFromFile(const std::string& filePath) {
    if (!isValidModuleFile(filePath)) {
        addError("无效的模块文件: " + filePath);
        return false;
    }
    
    auto module = createModuleFromFile(filePath);
    if (!module) {
        addError("无法创建模块: " + filePath);
        return false;
    }
    
    return registerModule(module);
}

bool CMODModuleManager::loadModulesFromDirectory(const std::string& directory) {
    auto moduleFiles = scanDirectory(directory);
    bool allLoaded = true;
    
    for (const auto& file : moduleFiles) {
        if (!loadModuleFromFile(file)) {
            allLoaded = false;
        }
    }
    
    return allLoaded;
}

// 依赖管理实现
std::vector<std::string> CMODModuleManager::resolveDependencies(const std::string& moduleName) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = modules.find(moduleName);
    if (it == modules.end()) {
        return {};
    }
    
    std::map<std::string, std::vector<std::string>> dependencyGraph;
    for (const auto& pair : modules) {
        dependencyGraph[pair.first] = pair.second->getDependencies();
    }
    
    return topologicalSort(dependencyGraph);
}

bool CMODModuleManager::checkDependencies(const std::string& moduleName) const {
    return isModuleDependencySatisfied(moduleName);
}

std::vector<std::string> CMODModuleManager::getDependencyChain(const std::string& moduleName) const {
    return resolveDependencies(moduleName);
}

// 模块搜索实现
std::vector<std::shared_ptr<CMODModule>> CMODModuleManager::searchModules(const std::string& query) const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::shared_ptr<CMODModule>> result;
    
    for (const auto& pair : modules) {
        if (matchesQuery(*pair.second, query)) {
            result.push_back(pair.second);
        }
    }
    
    return result;
}

std::vector<std::shared_ptr<CMODModule>> CMODModuleManager::searchModulesByAuthor(const std::string& author) const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::shared_ptr<CMODModule>> result;
    
    for (const auto& pair : modules) {
        if (matchesAuthor(*pair.second, author)) {
            result.push_back(pair.second);
        }
    }
    
    return result;
}

std::vector<std::shared_ptr<CMODModule>> CMODModuleManager::searchModulesByLicense(const std::string& license) const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::shared_ptr<CMODModule>> result;
    
    for (const auto& pair : modules) {
        if (matchesLicense(*pair.second, license)) {
            result.push_back(pair.second);
        }
    }
    
    return result;
}

std::vector<std::shared_ptr<CMODModule>> CMODModuleManager::searchModulesByTag(const std::string& tag) const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::shared_ptr<CMODModule>> result;
    
    for (const auto& pair : modules) {
        if (matchesTag(*pair.second, tag)) {
            result.push_back(pair.second);
        }
    }
    
    return result;
}

// 模块信息实现
std::vector<std::string> CMODModuleManager::getModuleNames() const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::string> names;
    for (const auto& pair : modules) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> CMODModuleManager::getLoadedModuleNames() const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::string> names;
    for (const auto& pair : modules) {
        if (pair.second->isLoaded()) {
            names.push_back(pair.first);
        }
    }
    return names;
}

std::vector<std::string> CMODModuleManager::getErrorModuleNames() const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::string> names;
    for (const auto& pair : modules) {
        if (pair.second->isError()) {
            names.push_back(pair.first);
        }
    }
    return names;
}

std::vector<std::string> CMODModuleManager::getDisabledModuleNames() const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::string> names;
    for (const auto& pair : modules) {
        if (pair.second->isDisabled()) {
            names.push_back(pair.first);
        }
    }
    return names;
}

// 统计信息实现
size_t CMODModuleManager::getModuleCount() const {
    std::lock_guard<std::mutex> lock(mutex);
    return modules.size();
}

size_t CMODModuleManager::getLoadedModuleCount() const {
    std::lock_guard<std::mutex> lock(mutex);
    size_t count = 0;
    for (const auto& pair : modules) {
        if (pair.second->isLoaded()) {
            count++;
        }
    }
    return count;
}

size_t CMODModuleManager::getErrorModuleCount() const {
    std::lock_guard<std::mutex> lock(mutex);
    size_t count = 0;
    for (const auto& pair : modules) {
        if (pair.second->isError()) {
            count++;
        }
    }
    return count;
}

size_t CMODModuleManager::getDisabledModuleCount() const {
    std::lock_guard<std::mutex> lock(mutex);
    size_t count = 0;
    for (const auto& pair : modules) {
        if (pair.second->isDisabled()) {
            count++;
        }
    }
    return count;
}

std::map<CMODModuleType, size_t> CMODModuleManager::getModuleCountByType() const {
    std::lock_guard<std::mutex> lock(mutex);
    std::map<CMODModuleType, size_t> counts;
    
    for (const auto& pair : modules) {
        counts[pair.second->getInfo().type]++;
    }
    
    return counts;
}

// 模块配置实现
void CMODModuleManager::setModuleDirectory(const std::string& directory) {
    std::lock_guard<std::mutex> lock(mutex);
    moduleDirectory = directory;
}

std::string CMODModuleManager::getModuleDirectory() const {
    std::lock_guard<std::mutex> lock(mutex);
    return moduleDirectory;
}

void CMODModuleManager::setAutoLoad(bool autoLoad) {
    std::lock_guard<std::mutex> lock(mutex);
    this->autoLoad = autoLoad;
}

bool CMODModuleManager::isAutoLoad() const {
    std::lock_guard<std::mutex> lock(mutex);
    return autoLoad;
}

void CMODModuleManager::setLoadOrder(const std::vector<std::string>& order) {
    std::lock_guard<std::mutex> lock(mutex);
    loadOrder = order;
}

std::vector<std::string> CMODModuleManager::getLoadOrder() const {
    std::lock_guard<std::mutex> lock(mutex);
    return loadOrder;
}

// 事件处理实现
void CMODModuleManager::setModuleLoadCallback(std::function<void(const std::string&)> callback) {
    std::lock_guard<std::mutex> lock(mutex);
    moduleLoadCallback = callback;
}

void CMODModuleManager::setModuleUnloadCallback(std::function<void(const std::string&)> callback) {
    std::lock_guard<std::mutex> lock(mutex);
    moduleUnloadCallback = callback;
}

void CMODModuleManager::setModuleErrorCallback(std::function<void(const std::string&, const std::string&)> callback) {
    std::lock_guard<std::mutex> lock(mutex);
    moduleErrorCallback = callback;
}

// 验证和诊断实现
bool CMODModuleManager::validateAllModules() const {
    std::lock_guard<std::mutex> lock(mutex);
    
    for (const auto& pair : modules) {
        if (!pair.second->validate()) {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> CMODModuleManager::getValidationErrors() const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::string> allErrors;
    
    for (const auto& pair : modules) {
        auto moduleErrors = pair.second->getValidationErrors();
        allErrors.insert(allErrors.end(), moduleErrors.begin(), moduleErrors.end());
    }
    
    return allErrors;
}

std::vector<std::string> CMODModuleManager::getModuleErrors(const std::string& moduleName) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = modules.find(moduleName);
    if (it == modules.end()) {
        return {};
    }
    
    return it->second->getErrors();
}

std::vector<std::string> CMODModuleManager::getModuleWarnings(const std::string& moduleName) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = modules.find(moduleName);
    if (it == modules.end()) {
        return {};
    }
    
    return it->second->getWarnings();
}

// 清理和重置实现
void CMODModuleManager::clear() {
    std::lock_guard<std::mutex> lock(mutex);
    
    for (const auto& pair : modules) {
        if (pair.second->isLoaded()) {
            pair.second->unload();
        }
    }
    
    modules.clear();
    errors.clear();
    warnings.clear();
}

void CMODModuleManager::reset() {
    clear();
}

// 错误处理实现
std::vector<std::string> CMODModuleManager::getErrors() const {
    std::lock_guard<std::mutex> lock(mutex);
    return errors;
}

std::vector<std::string> CMODModuleManager::getWarnings() const {
    std::lock_guard<std::mutex> lock(mutex);
    return warnings;
}

void CMODModuleManager::addError(const std::string& error) {
    std::lock_guard<std::mutex> lock(mutex);
    errors.push_back(error);
}

void CMODModuleManager::addWarning(const std::string& warning) {
    std::lock_guard<std::mutex> lock(mutex);
    warnings.push_back(warning);
}

void CMODModuleManager::clearErrors() {
    std::lock_guard<std::mutex> lock(mutex);
    errors.clear();
}

void CMODModuleManager::clearWarnings() {
    std::lock_guard<std::mutex> lock(mutex);
    warnings.clear();
}

// 内部方法实现
bool CMODModuleManager::loadModuleInternal(const std::string& name) {
    auto it = modules.find(name);
    if (it == modules.end()) {
        addError("模块不存在: " + name);
        return false;
    }
    
    if (!isModuleDependencySatisfied(name)) {
        addError("模块依赖未满足: " + name);
        return false;
    }
    
    if (it->second->load()) {
        notifyModuleLoaded(name);
        return true;
    } else {
        notifyModuleError(name, "加载失败");
        return false;
    }
}

bool CMODModuleManager::unloadModuleInternal(const std::string& name) {
    auto it = modules.find(name);
    if (it == modules.end()) {
        addError("模块不存在: " + name);
        return false;
    }
    
    if (it->second->unload()) {
        notifyModuleUnloaded(name);
        return true;
    } else {
        notifyModuleError(name, "卸载失败");
        return false;
    }
}

std::vector<std::string> CMODModuleManager::getModuleDependencies(const std::string& moduleName) const {
    auto it = modules.find(moduleName);
    if (it == modules.end()) {
        return {};
    }
    
    return it->second->getDependencies();
}

bool CMODModuleManager::isModuleDependencySatisfied(const std::string& moduleName) const {
    auto dependencies = getModuleDependencies(moduleName);
    
    for (const auto& dep : dependencies) {
        auto depIt = modules.find(dep);
        if (depIt == modules.end() || !depIt->second->isLoaded()) {
            return false;
        }
    }
    
    return true;
}

void CMODModuleManager::notifyModuleLoaded(const std::string& moduleName) {
    if (moduleLoadCallback) {
        moduleLoadCallback(moduleName);
    }
}

void CMODModuleManager::notifyModuleUnloaded(const std::string& moduleName) {
    if (moduleUnloadCallback) {
        moduleUnloadCallback(moduleName);
    }
}

void CMODModuleManager::notifyModuleError(const std::string& moduleName, const std::string& error) {
    if (moduleErrorCallback) {
        moduleErrorCallback(moduleName, error);
    }
}

// 模块发现实现
std::vector<std::string> CMODModuleManager::scanDirectory(const std::string& directory) const {
    std::vector<std::string> moduleFiles;
    
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file() && isValidModuleFile(entry.path().string())) {
                moduleFiles.push_back(entry.path().string());
            }
        }
    } catch (const std::exception& e) {
        // 在const方法中无法添加错误，这里只是记录
        // addError("扫描目录失败: " + std::string(e.what()));
    }
    
    return moduleFiles;
}

bool CMODModuleManager::isValidModuleFile(const std::string& filePath) const {
    std::string extension = std::filesystem::path(filePath).extension().string();
    return extension == ".cmod" || extension == ".json" || extension == ".xml";
}

std::shared_ptr<CMODModule> CMODModuleManager::createModuleFromFile(const std::string& filePath) const {
    // 简单的模块文件解析实现
    // 实际实现应该根据文件格式进行解析
    auto module = std::make_shared<CMODModule>();
    
    // 从文件路径提取模块名称
    std::string fileName = std::filesystem::path(filePath).stem().string();
    module->setMetadata("name", fileName);
    module->setMetadata("filePath", filePath);
    
    return module;
}

// 依赖解析实现
std::vector<std::string> CMODModuleManager::topologicalSort(const std::map<std::string, std::vector<std::string>>& dependencies) const {
    std::vector<std::string> result;
    std::map<std::string, int> inDegree;
    std::set<std::string> allModules;
    
    // 收集所有模块
    for (const auto& pair : dependencies) {
        allModules.insert(pair.first);
        for (const auto& dep : pair.second) {
            allModules.insert(dep);
        }
    }
    
    // 计算入度
    for (const auto& module : allModules) {
        inDegree[module] = 0;
    }
    
    for (const auto& pair : dependencies) {
        for (const auto& dep : pair.second) {
            inDegree[pair.first]++;
        }
    }
    
    // 拓扑排序
    std::queue<std::string> queue;
    for (const auto& pair : inDegree) {
        if (pair.second == 0) {
            queue.push(pair.first);
        }
    }
    
    while (!queue.empty()) {
        std::string current = queue.front();
        queue.pop();
        result.push_back(current);
        
        for (const auto& pair : dependencies) {
            if (std::find(pair.second.begin(), pair.second.end(), current) != pair.second.end()) {
                inDegree[pair.first]--;
                if (inDegree[pair.first] == 0) {
                    queue.push(pair.first);
                }
            }
        }
    }
    
    return result;
}

bool CMODModuleManager::hasCircularDependency(const std::map<std::string, std::vector<std::string>>& dependencies) const {
    auto sorted = topologicalSort(dependencies);
    return sorted.size() != dependencies.size();
}

// 搜索辅助实现
bool CMODModuleManager::matchesQuery(const CMODModule& module, const std::string& query) const {
    const auto& info = module.getInfo();
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
    
    std::string lowerName = info.name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
    
    std::string lowerDescription = info.description;
    std::transform(lowerDescription.begin(), lowerDescription.end(), lowerDescription.begin(), ::tolower);
    
    return lowerName.find(lowerQuery) != std::string::npos || 
           lowerDescription.find(lowerQuery) != std::string::npos;
}

bool CMODModuleManager::matchesAuthor(const CMODModule& module, const std::string& author) const {
    return module.getInfo().author == author;
}

bool CMODModuleManager::matchesLicense(const CMODModule& module, const std::string& license) const {
    return module.getInfo().license == license;
}

bool CMODModuleManager::matchesTag(const CMODModule& module, const std::string& tag) const {
    // 检查元数据中的标签
    auto metadata = module.getAllMetadata();
    for (const auto& pair : metadata) {
        if (pair.first == "tags" && pair.second.find(tag) != std::string::npos) {
            return true;
        }
    }
    return false;
}

} // namespace Module
} // namespace CHTL