#include "HybridModuleSystem.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace CHTL {

// HybridModule 实现
HybridModule::HybridModule() {
    info.name = "";
    info.version = "1.0.0";
    info.description = "";
    info.author = "";
    info.license = "MIT";
    info.type = HybridModuleType::HYBRID;
}

void HybridModule::setInfo(const HybridModuleInfo& moduleInfo) {
    info = moduleInfo;
    updateModuleType();
    validateModule();
}

void HybridModule::setCMODModule(std::shared_ptr<CMODModule> cmod) {
    cmodModule = cmod;
    updateModuleType();
    validateModule();
}

void HybridModule::setCJMODModule(std::shared_ptr<CJMODModule> cjmod) {
    cjmodModule = cjmod;
    updateModuleType();
    validateModule();
}

void HybridModule::addDependency(const std::string& moduleName, const std::string& version) {
    std::string dep = version.empty() ? moduleName : moduleName + "@" + version;
    if (std::find(dependencies.begin(), dependencies.end(), dep) == dependencies.end()) {
        dependencies.push_back(dep);
        std::cout << "Added hybrid module dependency: " << dep << std::endl;
    }
}

void HybridModule::removeDependency(const std::string& moduleName) {
    dependencies.erase(std::remove_if(dependencies.begin(), dependencies.end(),
        [&moduleName](const std::string& dep) {
            return dep.find(moduleName) == 0;
        }), dependencies.end());
    std::cout << "Removed hybrid module dependency: " << moduleName << std::endl;
}

bool HybridModule::hasDependency(const std::string& moduleName) const {
    return std::find_if(dependencies.begin(), dependencies.end(),
        [&moduleName](const std::string& dep) {
            return dep.find(moduleName) == 0;
        }) != dependencies.end();
}

std::vector<std::string> HybridModule::getDependencies() const {
    return dependencies;
}

bool HybridModule::isValid() const {
    return validationErrors.empty();
}

std::vector<std::string> HybridModule::getValidationErrors() const {
    return validationErrors;
}

void HybridModule::validateModule() {
    validationErrors.clear();
    
    if (info.name.empty()) {
        validationErrors.push_back("Module name is required");
    }
    
    if (info.version.empty()) {
        validationErrors.push_back("Module version is required");
    }
    
    // 根据模块类型验证
    switch (info.type) {
        case HybridModuleType::CMOD_ONLY:
            if (!cmodModule) {
                validationErrors.push_back("CMOD module is required for CMOD_ONLY type");
            }
            break;
        case HybridModuleType::CJMOD_ONLY:
            if (!cjmodModule) {
                validationErrors.push_back("CJMOD module is required for CJMOD_ONLY type");
            }
            break;
        case HybridModuleType::HYBRID:
            if (!cmodModule && !cjmodModule) {
                validationErrors.push_back("At least one module part (CMOD or CJMOD) is required for HYBRID type");
            }
            break;
    }
    
    // 验证子模块
    if (cmodModule && !cmodModule->isValid()) {
        auto cmodErrors = cmodModule->getValidationErrors();
        for (const auto& error : cmodErrors) {
            validationErrors.push_back("CMOD: " + error);
        }
    }
    
    if (cjmodModule && !cjmodModule->isValid()) {
        auto cjmodErrors = cjmodModule->getValidationErrors();
        for (const auto& error : cjmodErrors) {
            validationErrors.push_back("CJMOD: " + error);
        }
    }
}

void HybridModule::updateModuleType() {
    bool hasCMOD = cmodModule != nullptr;
    bool hasCJMOD = cjmodModule != nullptr;
    
    if (hasCMOD && !hasCJMOD) {
        info.type = HybridModuleType::CMOD_ONLY;
    } else if (!hasCMOD && hasCJMOD) {
        info.type = HybridModuleType::CJMOD_ONLY;
    } else if (hasCMOD && hasCJMOD) {
        info.type = HybridModuleType::HYBRID;
    }
}

std::string HybridModule::toJSON() const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"name\": \"" << info.name << "\",\n";
    ss << "  \"version\": \"" << info.version << "\",\n";
    ss << "  \"description\": \"" << info.description << "\",\n";
    ss << "  \"author\": \"" << info.author << "\",\n";
    ss << "  \"license\": \"" << info.license << "\",\n";
    ss << "  \"type\": " << static_cast<int>(info.type) << ",\n";
    ss << "  \"dependencies\": [";
    for (size_t i = 0; i < dependencies.size(); ++i) {
        ss << "\"" << dependencies[i] << "\"";
        if (i < dependencies.size() - 1) ss << ", ";
    }
    ss << "],\n";
    ss << "  \"hasCMOD\": " << (hasCMODPart() ? "true" : "false") << ",\n";
    ss << "  \"hasCJMOD\": " << (hasCJMODPart() ? "true" : "false") << "\n";
    ss << "}";
    return ss.str();
}

std::string HybridModule::toString() const {
    std::stringstream ss;
    ss << "Hybrid Module: " << info.name << " v" << info.version << "\n";
    ss << "Type: ";
    switch (info.type) {
        case HybridModuleType::CMOD_ONLY: ss << "CMOD_ONLY"; break;
        case HybridModuleType::CJMOD_ONLY: ss << "CJMOD_ONLY"; break;
        case HybridModuleType::HYBRID: ss << "HYBRID"; break;
    }
    ss << "\n";
    ss << "Description: " << info.description << "\n";
    ss << "Author: " << info.author << "\n";
    ss << "License: " << info.license << "\n";
    ss << "Dependencies: ";
    for (size_t i = 0; i < dependencies.size(); ++i) {
        ss << dependencies[i];
        if (i < dependencies.size() - 1) ss << ", ";
    }
    ss << "\n";
    ss << "CMOD Part: " << (hasCMODPart() ? "Yes" : "No") << "\n";
    ss << "CJMOD Part: " << (hasCJMODPart() ? "Yes" : "No") << "\n";
    return ss.str();
}

// HybridModuleSystem 实现
HybridModuleSystem::HybridModuleSystem() {
    // 初始化系统
}

void HybridModuleSystem::registerModule(std::shared_ptr<HybridModule> module) {
    if (module && !module->getInfo().name.empty()) {
        modules[module->getInfo().name] = module;
        std::cout << "Registered hybrid module: " << module->getInfo().name << std::endl;
        
        // 同时注册到子系统
        if (module->hasCMODPart()) {
            cmodSystem.registerModule(module->getCMODModule());
        }
        if (module->hasCJMODPart()) {
            cjmodSystem.registerModule(module->getCJMODModule());
        }
    }
}

void HybridModuleSystem::unregisterModule(const std::string& name) {
    auto it = modules.find(name);
    if (it != modules.end()) {
        auto module = it->second;
        
        // 从子系统注销
        if (module->hasCMODPart()) {
            cmodSystem.unregisterModule(module->getCMODModule()->getInfo().name);
        }
        if (module->hasCJMODPart()) {
            cjmodSystem.unregisterModule(module->getCJMODModule()->getInfo().name);
        }
        
        modules.erase(it);
        modulePaths.erase(name);
        std::cout << "Unregistered hybrid module: " << name << std::endl;
    }
}

bool HybridModuleSystem::hasModule(const std::string& name) const {
    return modules.find(name) != modules.end();
}

std::shared_ptr<HybridModule> HybridModuleSystem::getModule(const std::string& name) const {
    auto it = modules.find(name);
    if (it != modules.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::string> HybridModuleSystem::getAllModuleNames() const {
    std::vector<std::string> result;
    for (const auto& pair : modules) {
        result.push_back(pair.first);
    }
    return result;
}

std::vector<std::string> HybridModuleSystem::searchModules(const std::string& query) const {
    std::vector<std::string> result;
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
    
    for (const auto& pair : modules) {
        std::string moduleName = pair.first;
        std::string lowerName = moduleName;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        
        if (lowerName.find(lowerQuery) != std::string::npos) {
            result.push_back(moduleName);
        }
    }
    return result;
}

std::vector<std::string> HybridModuleSystem::findModulesByType(HybridModuleType type) const {
    std::vector<std::string> result;
    for (const auto& pair : modules) {
        if (pair.second->getInfo().type == type) {
            result.push_back(pair.first);
        }
    }
    return result;
}

std::vector<std::string> HybridModuleSystem::resolveDependencies(const std::string& moduleName) const {
    std::vector<std::string> resolved;
    std::set<std::string> visited;
    
    std::function<void(const std::string&)> resolve = [&](const std::string& name) {
        if (visited.count(name)) return;
        visited.insert(name);
        
        auto module = getModule(name);
        if (module) {
            auto deps = module->getDependencies();
            for (const auto& dep : deps) {
                resolve(dep);
            }
            resolved.push_back(name);
        }
    };
    
    resolve(moduleName);
    return resolved;
}

bool HybridModuleSystem::checkDependencyConflicts(const std::string& moduleName) const {
    auto module = getModule(moduleName);
    if (!module) return false;
    
    auto deps = module->getDependencies();
    for (const auto& dep : deps) {
        if (!hasModule(dep)) {
            std::cerr << "Missing hybrid module dependency: " << dep << std::endl;
            return true;
        }
    }
    return false;
}

std::shared_ptr<HybridModule> HybridModuleSystem::loadModule(const std::string& path) {
    try {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Cannot open hybrid module file: " << path << std::endl;
            return nullptr;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        file.close();
        
        // 检测模块类型
        HybridModuleType type = detectModuleType(content);
        
        auto module = std::make_shared<HybridModule>();
        
        // 从路径提取模块名
        std::filesystem::path filePath(path);
        std::string moduleName = filePath.stem().string();
        
        HybridModuleInfo info;
        info.name = moduleName;
        info.version = "1.0.0";
        info.description = "Loaded from " + path;
        info.type = type;
        module->setInfo(info);
        
        // 根据类型加载相应的子模块
        if (type == HybridModuleType::CMOD_ONLY || type == HybridModuleType::HYBRID) {
            auto cmod = cmodSystem.loadModule(path);
            if (cmod) {
                module->setCMODModule(cmod);
            }
        }
        
        if (type == HybridModuleType::CJMOD_ONLY || type == HybridModuleType::HYBRID) {
            auto cjmod = cjmodSystem.loadModule(path);
            if (cjmod) {
                module->setCJMODModule(cjmod);
            }
        }
        
        modulePaths[moduleName] = path;
        return module;
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading hybrid module: " << e.what() << std::endl;
        return nullptr;
    }
}

bool HybridModuleSystem::saveModule(std::shared_ptr<HybridModule> module, const std::string& path) {
    try {
        std::ofstream file(path);
        if (!file.is_open()) {
            std::cerr << "Cannot create hybrid module file: " << path << std::endl;
            return false;
        }
        
        // 保存混合模块内容
        std::string content = "";
        if (module->hasCMODPart()) {
            content += module->getCMODModule()->getSource();
        }
        if (module->hasCJMODPart()) {
            content += "\n\n" + module->getCJMODModule()->getSource();
        }
        
        file << content;
        file.close();
        
        modulePaths[module->getInfo().name] = path;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error saving hybrid module: " << e.what() << std::endl;
        return false;
    }
}

bool HybridModuleSystem::callFunction(const std::string& moduleName, const std::string& functionName, 
                                     const std::vector<std::string>& args) {
    auto module = getModule(moduleName);
    if (!module || !module->hasCJMODPart()) {
        std::cerr << "Module not found or no CJMOD part: " << moduleName << std::endl;
        return false;
    }
    
    return cjmodSystem.callFunction(module->getCJMODModule()->getInfo().name, functionName, args);
}

std::vector<ExportItem> HybridModuleSystem::getExports(const std::string& moduleName) const {
    auto module = getModule(moduleName);
    if (!module || !module->hasCMODPart()) {
        return {};
    }
    
    return module->getCMODModule()->getAllExports();
}

std::map<std::string, HybridModuleInfo> HybridModuleSystem::getAllModuleInfo() const {
    std::map<std::string, HybridModuleInfo> result;
    for (const auto& pair : modules) {
        result[pair.first] = pair.second->getInfo();
    }
    return result;
}

std::vector<std::string> HybridModuleSystem::findModuleFiles(const std::string& directory) const {
    std::vector<std::string> result;
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                if (ext == ".chtl" || ext == ".cjjs") {
                    result.push_back(entry.path().string());
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error scanning directory: " << e.what() << std::endl;
    }
    return result;
}

std::shared_ptr<HybridModule> HybridModuleSystem::parseModuleFile(const std::string& path) const {
    // 这里应该使用完整的解析器来解析混合模块文件
    return std::make_shared<HybridModule>();
}

HybridModuleType HybridModuleSystem::detectModuleType(const std::string& content) const {
    // 简化的类型检测，实际实现会更复杂
    bool hasCHTL = content.find("[Template]") != std::string::npos || 
                   content.find("[Custom]") != std::string::npos ||
                   content.find("html") != std::string::npos;
    bool hasCHTLJS = content.find("fileloader") != std::string::npos ||
                     content.find("listen") != std::string::npos ||
                     content.find("{{") != std::string::npos;
    
    if (hasCHTL && hasCHTLJS) {
        return HybridModuleType::HYBRID;
    } else if (hasCHTL) {
        return HybridModuleType::CMOD_ONLY;
    } else if (hasCHTLJS) {
        return HybridModuleType::CJMOD_ONLY;
    }
    
    return HybridModuleType::HYBRID; // 默认
}

} // namespace CHTL