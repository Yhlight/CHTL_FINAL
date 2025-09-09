#include "CMODSystem.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace CHTL {

// CMODModule 实现
CMODModule::CMODModule() : isPublic(false) {
    info.name = "";
    info.version = "1.0.0";
    info.description = "";
    info.author = "";
    info.license = "MIT";
}

void CMODModule::setInfo(const CMODInfo& moduleInfo) {
    info = moduleInfo;
    validateModule();
}

void CMODModule::setSource(const std::string& moduleSource) {
    source = moduleSource;
    validateModule();
}

void CMODModule::addExport(const ExportItem& item) {
    exports[item.name] = item;
    std::cout << "Added export: " << item.name << " (type: " << static_cast<int>(item.type) << ")" << std::endl;
}

void CMODModule::removeExport(const std::string& name) {
    auto it = exports.find(name);
    if (it != exports.end()) {
        exports.erase(it);
        std::cout << "Removed export: " << name << std::endl;
    }
}

bool CMODModule::hasExport(const std::string& name) const {
    return exports.find(name) != exports.end();
}

ExportItem CMODModule::getExport(const std::string& name) const {
    auto it = exports.find(name);
    if (it != exports.end()) {
        return it->second;
    }
    
    // 返回空导出项
    ExportItem emptyItem;
    emptyItem.type = ExportType::Template;
    emptyItem.name = "";
    emptyItem.description = "";
    emptyItem.node = nullptr;
    emptyItem.isPublic = false;
    return emptyItem;
}

std::vector<ExportItem> CMODModule::getAllExports() const {
    std::vector<ExportItem> result;
    for (const auto& pair : exports) {
        result.push_back(pair.second);
    }
    return result;
}

std::vector<ExportItem> CMODModule::getPublicExports() const {
    std::vector<ExportItem> result;
    for (const auto& pair : exports) {
        if (pair.second.isPublic) {
            result.push_back(pair.second);
        }
    }
    return result;
}

void CMODModule::addSubmodule(const std::string& name, std::shared_ptr<CMODModule> submodule) {
    submodules[name] = submodule;
    std::cout << "Added submodule: " << name << std::endl;
}

void CMODModule::removeSubmodule(const std::string& name) {
    auto it = submodules.find(name);
    if (it != submodules.end()) {
        submodules.erase(it);
        std::cout << "Removed submodule: " << name << std::endl;
    }
}

bool CMODModule::hasSubmodule(const std::string& name) const {
    return submodules.find(name) != submodules.end();
}

std::shared_ptr<CMODModule> CMODModule::getSubmodule(const std::string& name) const {
    auto it = submodules.find(name);
    if (it != submodules.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::string> CMODModule::getSubmoduleNames() const {
    std::vector<std::string> result;
    for (const auto& pair : submodules) {
        result.push_back(pair.first);
    }
    return result;
}

void CMODModule::addDependency(const std::string& moduleName, const std::string& version) {
    std::string dep = version.empty() ? moduleName : moduleName + "@" + version;
    if (std::find(dependencies.begin(), dependencies.end(), dep) == dependencies.end()) {
        dependencies.push_back(dep);
        std::cout << "Added dependency: " << dep << std::endl;
    }
}

void CMODModule::removeDependency(const std::string& moduleName) {
    dependencies.erase(std::remove_if(dependencies.begin(), dependencies.end(),
        [&moduleName](const std::string& dep) {
            return dep.find(moduleName) == 0;
        }), dependencies.end());
    std::cout << "Removed dependency: " << moduleName << std::endl;
}

bool CMODModule::hasDependency(const std::string& moduleName) const {
    return std::find_if(dependencies.begin(), dependencies.end(),
        [&moduleName](const std::string& dep) {
            return dep.find(moduleName) == 0;
        }) != dependencies.end();
}

std::vector<std::string> CMODModule::getDependencies() const {
    return dependencies;
}

bool CMODModule::isValid() const {
    return validationErrors.empty();
}

std::vector<std::string> CMODModule::getValidationErrors() const {
    return validationErrors;
}

void CMODModule::validateModule() {
    validationErrors.clear();
    
    if (info.name.empty()) {
        validationErrors.push_back("Module name is required");
    }
    
    if (info.version.empty()) {
        validationErrors.push_back("Module version is required");
    }
    
    if (source.empty()) {
        validationErrors.push_back("Module source is required");
    }
    
    // 检查导出项是否有重复名称
    std::set<std::string> exportNames;
    for (const auto& pair : exports) {
        if (exportNames.count(pair.first)) {
            validationErrors.push_back("Duplicate export name: " + pair.first);
        }
        exportNames.insert(pair.first);
    }
}

std::string CMODModule::toJSON() const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"name\": \"" << info.name << "\",\n";
    ss << "  \"version\": \"" << info.version << "\",\n";
    ss << "  \"description\": \"" << info.description << "\",\n";
    ss << "  \"author\": \"" << info.author << "\",\n";
    ss << "  \"license\": \"" << info.license << "\",\n";
    ss << "  \"dependencies\": [";
    for (size_t i = 0; i < dependencies.size(); ++i) {
        ss << "\"" << dependencies[i] << "\"";
        if (i < dependencies.size() - 1) ss << ", ";
    }
    ss << "],\n";
    ss << "  \"exports\": [";
    size_t exportCount = 0;
    for (const auto& pair : exports) {
        if (exportCount > 0) ss << ", ";
        ss << "{\n";
        ss << "    \"name\": \"" << pair.second.name << "\",\n";
        ss << "    \"type\": " << static_cast<int>(pair.second.type) << ",\n";
        ss << "    \"description\": \"" << pair.second.description << "\",\n";
        ss << "    \"isPublic\": " << (pair.second.isPublic ? "true" : "false") << "\n";
        ss << "  }";
        exportCount++;
    }
    ss << "]\n";
    ss << "}";
    return ss.str();
}

std::string CMODModule::toString() const {
    std::stringstream ss;
    ss << "CMOD Module: " << info.name << " v" << info.version << "\n";
    ss << "Description: " << info.description << "\n";
    ss << "Author: " << info.author << "\n";
    ss << "License: " << info.license << "\n";
    ss << "Dependencies: ";
    for (size_t i = 0; i < dependencies.size(); ++i) {
        ss << dependencies[i];
        if (i < dependencies.size() - 1) ss << ", ";
    }
    ss << "\n";
    ss << "Exports (" << exports.size() << "):\n";
    for (const auto& pair : exports) {
        ss << "  - " << pair.second.name << " (" << static_cast<int>(pair.second.type) << ")\n";
    }
    ss << "Submodules (" << submodules.size() << "):\n";
    for (const auto& pair : submodules) {
        ss << "  - " << pair.first << "\n";
    }
    return ss.str();
}

// CMODSystem 实现
CMODSystem::CMODSystem() {
    // 初始化系统
}

void CMODSystem::registerModule(std::shared_ptr<CMODModule> module) {
    if (module && !module->getInfo().name.empty()) {
        modules[module->getInfo().name] = module;
        std::cout << "Registered module: " << module->getInfo().name << std::endl;
    }
}

void CMODSystem::unregisterModule(const std::string& name) {
    auto it = modules.find(name);
    if (it != modules.end()) {
        modules.erase(it);
        modulePaths.erase(name);
        std::cout << "Unregistered module: " << name << std::endl;
    }
}

bool CMODSystem::hasModule(const std::string& name) const {
    return modules.find(name) != modules.end();
}

std::shared_ptr<CMODModule> CMODSystem::getModule(const std::string& name) const {
    auto it = modules.find(name);
    if (it != modules.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::string> CMODSystem::getAllModuleNames() const {
    std::vector<std::string> result;
    for (const auto& pair : modules) {
        result.push_back(pair.first);
    }
    return result;
}

std::vector<std::string> CMODSystem::searchModules(const std::string& query) const {
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

std::vector<std::string> CMODSystem::findModulesByType(ExportType type) const {
    std::vector<std::string> result;
    for (const auto& pair : modules) {
        auto exports = pair.second->getAllExports();
        for (const auto& exportItem : exports) {
            if (exportItem.type == type) {
                result.push_back(pair.first);
                break;
            }
        }
    }
    return result;
}

std::vector<std::string> CMODSystem::resolveDependencies(const std::string& moduleName) const {
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

bool CMODSystem::checkDependencyConflicts(const std::string& moduleName) const {
    // 简化的冲突检查，实际实现会更复杂
    auto module = getModule(moduleName);
    if (!module) return false;
    
    auto deps = module->getDependencies();
    for (const auto& dep : deps) {
        if (!hasModule(dep)) {
            std::cerr << "Missing dependency: " << dep << std::endl;
            return true;
        }
    }
    return false;
}

std::shared_ptr<CMODModule> CMODSystem::loadModule(const std::string& path) {
    try {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Cannot open module file: " << path << std::endl;
            return nullptr;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        file.close();
        
        // 这里应该解析CHTL文件并创建模块
        // 简化实现，实际需要完整的解析器
        auto module = std::make_shared<CMODModule>();
        module->setSource(content);
        
        // 从路径提取模块名
        std::filesystem::path filePath(path);
        std::string moduleName = filePath.stem().string();
        
        CMODInfo info;
        info.name = moduleName;
        info.version = "1.0.0";
        info.description = "Loaded from " + path;
        module->setInfo(info);
        
        modulePaths[moduleName] = path;
        return module;
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading module: " << e.what() << std::endl;
        return nullptr;
    }
}

bool CMODSystem::saveModule(std::shared_ptr<CMODModule> module, const std::string& path) {
    try {
        std::ofstream file(path);
        if (!file.is_open()) {
            std::cerr << "Cannot create module file: " << path << std::endl;
            return false;
        }
        
        file << module->getSource();
        file.close();
        
        modulePaths[module->getInfo().name] = path;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error saving module: " << e.what() << std::endl;
        return false;
    }
}

std::map<std::string, CMODInfo> CMODSystem::getAllModuleInfo() const {
    std::map<std::string, CMODInfo> result;
    for (const auto& pair : modules) {
        result[pair.first] = pair.second->getInfo();
    }
    return result;
}

std::vector<std::string> CMODSystem::findModuleFiles(const std::string& directory) const {
    std::vector<std::string> result;
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".chtl") {
                result.push_back(entry.path().string());
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error scanning directory: " << e.what() << std::endl;
    }
    return result;
}

std::shared_ptr<CMODModule> CMODSystem::parseModuleFile(const std::string& path) const {
    // 这里应该使用CHTL解析器来解析模块文件
    // 简化实现，返回空模块
    return std::make_shared<CMODModule>();
}

} // namespace CHTL