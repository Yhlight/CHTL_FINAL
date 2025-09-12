#include "CHTL/ModuleManager.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <fstream>

namespace CHTL {

ModuleManager::ModuleManager(std::shared_ptr<CHTLContext> context)
    : m_context(context)
    , m_debugMode(false)
    , m_totalModules(0)
    , m_loadedModulesCount(0)
    , m_failedModulesCount(0)
    , m_dependencyConflicts(0)
{
}

ModuleManager::~ModuleManager() {
}

bool ModuleManager::addModule(std::shared_ptr<ModuleNode> moduleNode) {
    if (!moduleNode) return false;
    
    m_totalModules++;
    
    if (m_debugMode) {
        std::cout << "[ModuleManager] Adding module: " << moduleNode->getModuleName() << std::endl;
    }
    
    // 检查模块是否已存在
    std::string name = moduleNode->getModuleName();
    if (m_moduleMap.find(name) != m_moduleMap.end()) {
        addWarning("Module already exists: " + name);
        return false;
    }
    
    // 添加模块
    m_modules.push_back(moduleNode);
    m_moduleMap[name] = moduleNode;
    
    return true;
}

std::shared_ptr<ModuleNode> ModuleManager::getModule(const std::string& name) {
    auto it = m_moduleMap.find(name);
    if (it != m_moduleMap.end()) {
        return it->second;
    }
    return nullptr;
}

bool ModuleManager::hasModule(const std::string& name) {
    return m_moduleMap.find(name) != m_moduleMap.end();
}

const std::vector<std::shared_ptr<ModuleNode>>& ModuleManager::getAllModules() const {
    return m_modules;
}

std::vector<std::shared_ptr<ModuleNode>> ModuleManager::getModulesByType(ModuleType type) {
    std::vector<std::shared_ptr<ModuleNode>> result;
    for (const auto& module : m_modules) {
        if (module->getModuleType() == type) {
            result.push_back(module);
        }
    }
    return result;
}

std::vector<std::shared_ptr<ModuleNode>> ModuleManager::getCMODModules() {
    return getModulesByType(ModuleType::CMOD);
}

std::vector<std::shared_ptr<ModuleNode>> ModuleManager::getCJMODModules() {
    return getModulesByType(ModuleType::CJMOD);
}

bool ModuleManager::resolveModuleDependencies(const std::string& moduleName) {
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> resolving;
    
    return resolveModuleDependenciesRecursive(moduleName, visited, resolving);
}

bool ModuleManager::resolveAllModuleDependencies() {
    bool allSuccess = true;
    
    for (const auto& module : m_modules) {
        if (!resolveModuleDependencies(module->getModuleName())) {
            allSuccess = false;
        }
    }
    
    return allSuccess;
}

bool ModuleManager::loadModule(const std::string& moduleName) {
    auto module = getModule(moduleName);
    if (!module) {
        addError("Module not found: " + moduleName);
        return false;
    }
    
    if (m_loadedModules.find(moduleName) != m_loadedModules.end()) {
        addWarning("Module already loaded: " + moduleName);
        return true;
    }
    
    if (m_debugMode) {
        std::cout << "[ModuleManager] Loading module: " << moduleName << std::endl;
    }
    
    // 解析依赖
    if (!resolveModuleDependencies(moduleName)) {
        addError("Failed to resolve dependencies for module: " + moduleName);
        return false;
    }
    
    // 加载模块内容
    if (!loadModuleContent(module)) {
        addError("Failed to load content for module: " + moduleName);
        return false;
    }
    
    m_loadedModules.insert(moduleName);
    m_loadedModulesCount++;
    
    return true;
}

bool ModuleManager::loadAllModules() {
    bool allSuccess = true;
    
    for (const auto& module : m_modules) {
        if (!loadModule(module->getModuleName())) {
            allSuccess = false;
        }
    }
    
    return allSuccess;
}

bool ModuleManager::unloadModule(const std::string& moduleName) {
    if (m_loadedModules.find(moduleName) == m_loadedModules.end()) {
        addWarning("Module not loaded: " + moduleName);
        return false;
    }
    
    if (m_debugMode) {
        std::cout << "[ModuleManager] Unloading module: " << moduleName << std::endl;
    }
    
    m_loadedModules.erase(moduleName);
    m_loadedModulesCount--;
    
    return true;
}

bool ModuleManager::unloadAllModules() {
    m_loadedModules.clear();
    m_loadedModulesCount = 0;
    return true;
}

std::vector<std::string> ModuleManager::checkModuleDependencies(const std::string& moduleName) {
    std::vector<std::string> missingDependencies;
    
    auto module = getModule(moduleName);
    if (!module) {
        missingDependencies.push_back("Module not found: " + moduleName);
        return missingDependencies;
    }
    
    const auto& dependencies = module->getModuleDependencies();
    for (const auto& dep : dependencies) {
        if (!hasModule(dep.first)) {
            missingDependencies.push_back("Missing dependency: " + dep.first);
        }
    }
    
    return missingDependencies;
}

std::vector<std::string> ModuleManager::getModuleLoadOrder() {
    std::vector<std::string> loadOrder;
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> resolving;
    
    for (const auto& module : m_modules) {
        if (visited.find(module->getModuleName()) == visited.end()) {
            resolveModuleDependenciesRecursive(module->getModuleName(), visited, resolving);
        }
    }
    
    // 将解析顺序转换为加载顺序
    for (const auto& module : m_modules) {
        if (visited.find(module->getModuleName()) != visited.end()) {
            loadOrder.push_back(module->getModuleName());
        }
    }
    
    return loadOrder;
}

void ModuleManager::clear() {
    m_modules.clear();
    m_moduleMap.clear();
    m_loadedModules.clear();
    m_resolvedModules.clear();
    m_totalModules = 0;
    m_loadedModulesCount = 0;
    m_failedModulesCount = 0;
    m_dependencyConflicts = 0;
}

void ModuleManager::setDebugMode(bool debug) {
    m_debugMode = debug;
}

bool ModuleManager::isDebugMode() const {
    return m_debugMode;
}

std::string ModuleManager::getStatistics() const {
    std::ostringstream oss;
    oss << "Module Statistics:\n";
    oss << "  Total modules: " << m_totalModules << "\n";
    oss << "  Loaded: " << m_loadedModulesCount << "\n";
    oss << "  Failed: " << m_failedModulesCount << "\n";
    oss << "  Dependency conflicts: " << m_dependencyConflicts << "\n";
    oss << "  Success rate: " << (m_totalModules > 0 ? (m_loadedModulesCount * 100.0 / m_totalModules) : 0) << "%\n";
    return oss.str();
}

bool ModuleManager::resolveModuleDependenciesRecursive(const std::string& moduleName, 
                                                      std::unordered_set<std::string>& visited,
                                                      std::unordered_set<std::string>& resolving) {
    if (resolving.find(moduleName) != resolving.end()) {
        addError("Circular dependency detected: " + moduleName);
        m_dependencyConflicts++;
        return false;
    }
    
    if (visited.find(moduleName) != visited.end()) {
        return true;
    }
    
    auto module = getModule(moduleName);
    if (!module) {
        addError("Module not found: " + moduleName);
        return false;
    }
    
    resolving.insert(moduleName);
    
    const auto& dependencies = module->getModuleDependencies();
    for (const auto& dep : dependencies) {
        if (!resolveModuleDependenciesRecursive(dep.first, visited, resolving)) {
            resolving.erase(moduleName);
            return false;
        }
    }
    
    resolving.erase(moduleName);
    visited.insert(moduleName);
    m_resolvedModules.insert(moduleName);
    
    return true;
}

bool ModuleManager::loadModuleContent(std::shared_ptr<ModuleNode> moduleNode) {
    if (m_debugMode) {
        std::cout << "[ModuleManager] Loading content for module: " << moduleNode->getModuleName() << std::endl;
    }
    
    // 根据模块类型加载内容
    switch (moduleNode->getModuleType()) {
        case ModuleType::CMOD:
            // 加载CHTL模块内容
            if (!moduleNode->getModulePath().empty()) {
                std::ifstream file(moduleNode->getModulePath());
                if (file.is_open()) {
                    std::string content((std::istreambuf_iterator<char>(file)),
                                      std::istreambuf_iterator<char>());
                    moduleNode->setModuleContent(content);
                    file.close();
                } else {
                    addError("Failed to open CMOD file: " + moduleNode->getModulePath());
                    return false;
                }
            }
            break;
            
        case ModuleType::CJMOD:
            // 加载CHTL JS模块内容
            if (!moduleNode->getModulePath().empty()) {
                std::ifstream file(moduleNode->getModulePath());
                if (file.is_open()) {
                    std::string content((std::istreambuf_iterator<char>(file)),
                                      std::istreambuf_iterator<char>());
                    moduleNode->setModuleContent(content);
                    file.close();
                } else {
                    addError("Failed to open CJMOD file: " + moduleNode->getModulePath());
                    return false;
                }
            }
            break;
            
        case ModuleType::HTML:
        case ModuleType::CSS:
        case ModuleType::JS:
            // 加载其他类型模块内容
            if (!moduleNode->getModulePath().empty()) {
                std::ifstream file(moduleNode->getModulePath());
                if (file.is_open()) {
                    std::string content((std::istreambuf_iterator<char>(file)),
                                      std::istreambuf_iterator<char>());
                    moduleNode->setModuleContent(content);
                    file.close();
                } else {
                    addError("Failed to open module file: " + moduleNode->getModulePath());
                    return false;
                }
            }
            break;
            
        default:
            addError("Unknown module type: " + std::to_string(static_cast<int>(moduleNode->getModuleType())));
            return false;
    }
    
    return true;
}

void ModuleManager::addError(const std::string& message) {
    if (m_context) {
        m_context->addError("ModuleManager Error: " + message);
    }
}

void ModuleManager::addWarning(const std::string& message) {
    if (m_context) {
        m_context->addWarning("ModuleManager Warning: " + message);
    }
}

} // namespace CHTL