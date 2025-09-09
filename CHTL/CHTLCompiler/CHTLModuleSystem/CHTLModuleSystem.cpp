#include "CHTLModuleSystem.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <filesystem>

namespace CHTL {

// CHTLCMODManager implementation
CHTLCMODManager::CHTLCMODManager() {
}

CHTLCMODManager::~CHTLCMODManager() {
}

bool CHTLCMODManager::loadModule(const std::string& name, const std::string& path) {
    if (name.empty() || path.empty()) {
        return false;
    }
    
    std::string content = readModuleFile(path);
    if (content.empty()) {
        return false;
    }
    
    CMODModule module(name, content);
    module.path = path;
    
    // Parse module info
    if (!parseModuleInfo(content, module.info)) {
        return false;
    }
    
    // Parse exports and imports
    parseModuleExports(content, module.exports);
    parseModuleImports(content, module.imports);
    
    modules[name] = module;
    return true;
}

bool CHTLCMODManager::unloadModule(const std::string& name) {
    return modules.erase(name) > 0;
}

bool CHTLCMODManager::reloadModule(const std::string& name) {
    auto it = modules.find(name);
    if (it == modules.end()) {
        return false;
    }
    
    std::string path = it->second.path;
    unloadModule(name);
    return loadModule(name, path);
}

bool CHTLCMODManager::hasModule(const std::string& name) const {
    return modules.find(name) != modules.end();
}

CMODModule CHTLCMODManager::getModule(const std::string& name) const {
    auto it = modules.find(name);
    return (it != modules.end()) ? it->second : CMODModule();
}

std::vector<std::string> CHTLCMODManager::getModuleNames() const {
    std::vector<std::string> names;
    for (const auto& pair : modules) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> CHTLCMODManager::getModulePaths() const {
    std::vector<std::string> paths;
    for (const auto& pair : modules) {
        paths.push_back(pair.second.path);
    }
    return paths;
}

std::string CHTLCMODManager::getModuleContent(const std::string& name) const {
    auto it = modules.find(name);
    return (it != modules.end()) ? it->second.content : "";
}

std::string CHTLCMODManager::getModuleExport(const std::string& name, const std::string& export_name) const {
    auto it = modules.find(name);
    if (it != modules.end()) {
        auto export_it = it->second.exports.find(export_name);
        if (export_it != it->second.exports.end()) {
            return export_it->second;
        }
    }
    return "";
}

std::vector<std::string> CHTLCMODManager::getModuleExports(const std::string& name) const {
    std::vector<std::string> exports;
    auto it = modules.find(name);
    if (it != modules.end()) {
        for (const auto& pair : it->second.exports) {
            exports.push_back(pair.first);
        }
    }
    return exports;
}

std::vector<std::string> CHTLCMODManager::getModuleImports(const std::string& name) const {
    std::vector<std::string> imports;
    auto it = modules.find(name);
    if (it != modules.end()) {
        for (const auto& pair : it->second.imports) {
            imports.push_back(pair.first);
        }
    }
    return imports;
}

void CHTLCMODManager::addSearchPath(const std::string& path, bool recursive, int priority) {
    ModuleSearchPath search_path(path, recursive, priority);
    search_paths.push_back(search_path);
    
    // Sort by priority (higher priority first)
    std::sort(search_paths.begin(), search_paths.end(),
        [](const ModuleSearchPath& a, const ModuleSearchPath& b) {
            return a.priority > b.priority;
        });
}

void CHTLCMODManager::removeSearchPath(const std::string& path) {
    search_paths.erase(
        std::remove_if(search_paths.begin(), search_paths.end(),
            [&path](const ModuleSearchPath& sp) {
                return sp.path == path;
            }),
        search_paths.end());
}

std::vector<ModuleSearchPath> CHTLCMODManager::getSearchPaths() const {
    return search_paths;
}

std::vector<ModuleDependency> CHTLCMODManager::getModuleDependencies(const std::string& name) const {
    std::vector<ModuleDependency> dependencies;
    auto it = modules.find(name);
    if (it != modules.end()) {
        // Parse dependencies from module info
        for (const auto& dep_name : it->second.info.dependencies) {
            ModuleDependency dep(dep_name);
            dependencies.push_back(dep);
        }
    }
    return dependencies;
}

bool CHTLCMODManager::resolveDependencies(const std::string& name) {
    auto dependencies = getModuleDependencies(name);
    for (const auto& dep : dependencies) {
        if (!hasModule(dep.name)) {
            std::string dep_path = findModulePath(dep.name);
            if (dep_path.empty() || !loadModule(dep.name, dep_path)) {
                return false;
            }
        }
    }
    return true;
}

std::vector<std::string> CHTLCMODManager::getDependencyChain(const std::string& name) const {
    std::vector<std::string> chain;
    std::set<std::string> visited;
    
    std::function<void(const std::string&)> buildChain = [&](const std::string& module_name) {
        if (visited.find(module_name) != visited.end()) {
            return; // Avoid circular dependencies
        }
        
        visited.insert(module_name);
        auto dependencies = getModuleDependencies(module_name);
        
        for (const auto& dep : dependencies) {
            buildChain(dep.name);
        }
        
        chain.push_back(module_name);
    };
    
    buildChain(name);
    return chain;
}

bool CHTLCMODManager::validateModule(const std::string& name) const {
    auto it = modules.find(name);
    if (it == modules.end()) {
        return false;
    }
    
    // Basic validation
    return !it->second.content.empty() && !it->second.info.name.empty();
}

std::vector<std::string> CHTLCMODManager::getModuleErrors(const std::string& name) const {
    std::vector<std::string> errors;
    auto it = modules.find(name);
    if (it == modules.end()) {
        errors.push_back("Module not found: " + name);
        return errors;
    }
    
    if (it->second.content.empty()) {
        errors.push_back("Module content is empty");
    }
    
    if (it->second.info.name.empty()) {
        errors.push_back("Module name is empty");
    }
    
    return errors;
}

bool CHTLCMODManager::validateAllModules() const {
    for (const auto& pair : modules) {
        if (!validateModule(pair.first)) {
            return false;
        }
    }
    return true;
}

std::string CHTLCMODManager::findModulePath(const std::string& name) const {
    for (const auto& search_path : search_paths) {
        std::string full_path = search_path.path + "/" + name;
        if (isModuleFile(full_path)) {
            return full_path;
        }
        
        if (search_path.recursive) {
            auto files = findModuleFiles(search_path.path);
            for (const auto& file : files) {
                if (file.find(name) != std::string::npos) {
                    return file;
                }
            }
        }
    }
    return "";
}

std::string CHTLCMODManager::generateModuleId(const std::string& name) const {
    return "cmod_" + name;
}

void CHTLCMODManager::clearCache() {
    modules.clear();
}

bool CHTLCMODManager::parseModuleInfo(const std::string& content, ModuleInfo& info) const {
    // Simple parsing - look for module info comments
    std::regex info_regex(R"(//\s*@module\s+(\w+))");
    std::smatch match;
    
    if (std::regex_search(content, match, info_regex)) {
        info.name = match[1].str();
        return true;
    }
    
    return false;
}

bool CHTLCMODManager::parseModuleExports(const std::string& content, std::map<std::string, std::string>& exports) const {
    // Simple parsing - look for export statements
    std::regex export_regex(R"(//\s*@export\s+(\w+)\s*=\s*(.+))");
    std::sregex_iterator iter(content.begin(), content.end(), export_regex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        const std::smatch& match = *iter;
        exports[match[1].str()] = match[2].str();
    }
    
    return true;
}

bool CHTLCMODManager::parseModuleImports(const std::string& content, std::map<std::string, std::string>& imports) const {
    // Simple parsing - look for import statements
    std::regex import_regex(R"(//\s*@import\s+(\w+)\s*from\s*(.+))");
    std::sregex_iterator iter(content.begin(), content.end(), import_regex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        const std::smatch& match = *iter;
        imports[match[1].str()] = match[2].str();
    }
    
    return true;
}

std::string CHTLCMODManager::readModuleFile(const std::string& path) const {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool CHTLCMODManager::isModuleFile(const std::string& path) const {
    return path.length() >= 5 && (path.substr(path.length() - 5) == ".chtl" || path.substr(path.length() - 5) == ".cmod");
}

std::vector<std::string> CHTLCMODManager::findModuleFiles(const std::string& /* directory */) const {
    std::vector<std::string> files;
    // Simplified implementation - would use filesystem in real implementation
    return files;
}

bool CHTLCMODManager::hasCircularDependency(const std::string& name, const std::set<std::string>& visited) const {
    if (visited.find(name) != visited.end()) {
        return true;
    }
    
    auto dependencies = getModuleDependencies(name);
    std::set<std::string> new_visited = visited;
    new_visited.insert(name);
    
    for (const auto& dep : dependencies) {
        if (hasCircularDependency(dep.name, new_visited)) {
            return true;
        }
    }
    
    return false;
}

// CHTLCJMODManager implementation
CHTLCJMODManager::CHTLCJMODManager() {
}

CHTLCJMODManager::~CHTLCJMODManager() {
}

bool CHTLCJMODManager::loadModule(const std::string& name, const std::string& path) {
    if (name.empty() || path.empty()) {
        return false;
    }
    
    std::string content = readModuleFile(path);
    if (content.empty()) {
        return false;
    }
    
    CJMODModule module(name, content);
    module.path = path;
    
    // Parse module info
    if (!parseModuleInfo(content, module.info)) {
        return false;
    }
    
    // Parse exports and imports
    parseModuleExports(content, module.exports);
    parseModuleImports(content, module.imports);
    
    modules[name] = module;
    return true;
}

bool CHTLCJMODManager::unloadModule(const std::string& name) {
    return modules.erase(name) > 0;
}

bool CHTLCJMODManager::reloadModule(const std::string& name) {
    auto it = modules.find(name);
    if (it == modules.end()) {
        return false;
    }
    
    std::string path = it->second.path;
    unloadModule(name);
    return loadModule(name, path);
}

bool CHTLCJMODManager::hasModule(const std::string& name) const {
    return modules.find(name) != modules.end();
}

CJMODModule CHTLCJMODManager::getModule(const std::string& name) const {
    auto it = modules.find(name);
    return (it != modules.end()) ? it->second : CJMODModule();
}

std::vector<std::string> CHTLCJMODManager::getModuleNames() const {
    std::vector<std::string> names;
    for (const auto& pair : modules) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> CHTLCJMODManager::getModulePaths() const {
    std::vector<std::string> paths;
    for (const auto& pair : modules) {
        paths.push_back(pair.second.path);
    }
    return paths;
}

std::string CHTLCJMODManager::getModuleContent(const std::string& name) const {
    auto it = modules.find(name);
    return (it != modules.end()) ? it->second.content : "";
}

std::string CHTLCJMODManager::getModuleExport(const std::string& name, const std::string& export_name) const {
    auto it = modules.find(name);
    if (it != modules.end()) {
        auto export_it = it->second.exports.find(export_name);
        if (export_it != it->second.exports.end()) {
            return export_it->second;
        }
    }
    return "";
}

std::vector<std::string> CHTLCJMODManager::getModuleExports(const std::string& name) const {
    std::vector<std::string> exports;
    auto it = modules.find(name);
    if (it != modules.end()) {
        for (const auto& pair : it->second.exports) {
            exports.push_back(pair.first);
        }
    }
    return exports;
}

std::vector<std::string> CHTLCJMODManager::getModuleImports(const std::string& name) const {
    std::vector<std::string> imports;
    auto it = modules.find(name);
    if (it != modules.end()) {
        for (const auto& pair : it->second.imports) {
            imports.push_back(pair.first);
        }
    }
    return imports;
}

void CHTLCJMODManager::addSearchPath(const std::string& path, bool recursive, int priority) {
    ModuleSearchPath search_path(path, recursive, priority);
    search_paths.push_back(search_path);
    
    // Sort by priority (higher priority first)
    std::sort(search_paths.begin(), search_paths.end(),
        [](const ModuleSearchPath& a, const ModuleSearchPath& b) {
            return a.priority > b.priority;
        });
}

void CHTLCJMODManager::removeSearchPath(const std::string& path) {
    search_paths.erase(
        std::remove_if(search_paths.begin(), search_paths.end(),
            [&path](const ModuleSearchPath& sp) {
                return sp.path == path;
            }),
        search_paths.end());
}

std::vector<ModuleSearchPath> CHTLCJMODManager::getSearchPaths() const {
    return search_paths;
}

std::vector<ModuleDependency> CHTLCJMODManager::getModuleDependencies(const std::string& name) const {
    std::vector<ModuleDependency> dependencies;
    auto it = modules.find(name);
    if (it != modules.end()) {
        // Parse dependencies from module info
        for (const auto& dep_name : it->second.info.dependencies) {
            ModuleDependency dep(dep_name);
            dependencies.push_back(dep);
        }
    }
    return dependencies;
}

bool CHTLCJMODManager::resolveDependencies(const std::string& name) {
    auto dependencies = getModuleDependencies(name);
    for (const auto& dep : dependencies) {
        if (!hasModule(dep.name)) {
            std::string dep_path = findModulePath(dep.name);
            if (dep_path.empty() || !loadModule(dep.name, dep_path)) {
                return false;
            }
        }
    }
    return true;
}

std::vector<std::string> CHTLCJMODManager::getDependencyChain(const std::string& name) const {
    std::vector<std::string> chain;
    std::set<std::string> visited;
    
    std::function<void(const std::string&)> buildChain = [&](const std::string& module_name) {
        if (visited.find(module_name) != visited.end()) {
            return; // Avoid circular dependencies
        }
        
        visited.insert(module_name);
        auto dependencies = getModuleDependencies(module_name);
        
        for (const auto& dep : dependencies) {
            buildChain(dep.name);
        }
        
        chain.push_back(module_name);
    };
    
    buildChain(name);
    return chain;
}

bool CHTLCJMODManager::validateModule(const std::string& name) const {
    auto it = modules.find(name);
    if (it == modules.end()) {
        return false;
    }
    
    // Basic validation
    return !it->second.content.empty() && !it->second.info.name.empty();
}

std::vector<std::string> CHTLCJMODManager::getModuleErrors(const std::string& name) const {
    std::vector<std::string> errors;
    auto it = modules.find(name);
    if (it == modules.end()) {
        errors.push_back("Module not found: " + name);
        return errors;
    }
    
    if (it->second.content.empty()) {
        errors.push_back("Module content is empty");
    }
    
    if (it->second.info.name.empty()) {
        errors.push_back("Module name is empty");
    }
    
    return errors;
}

bool CHTLCJMODManager::validateAllModules() const {
    for (const auto& pair : modules) {
        if (!validateModule(pair.first)) {
            return false;
        }
    }
    return true;
}

std::string CHTLCJMODManager::findModulePath(const std::string& name) const {
    for (const auto& search_path : search_paths) {
        std::string full_path = search_path.path + "/" + name;
        if (isModuleFile(full_path)) {
            return full_path;
        }
        
        if (search_path.recursive) {
            auto files = findModuleFiles(search_path.path);
            for (const auto& file : files) {
                if (file.find(name) != std::string::npos) {
                    return file;
                }
            }
        }
    }
    return "";
}

std::string CHTLCJMODManager::generateModuleId(const std::string& name) const {
    return "cjmod_" + name;
}

void CHTLCJMODManager::clearCache() {
    modules.clear();
}

bool CHTLCJMODManager::parseModuleInfo(const std::string& content, ModuleInfo& info) const {
    // Simple parsing - look for module info comments
    std::regex info_regex(R"(//\s*@module\s+(\w+))");
    std::smatch match;
    
    if (std::regex_search(content, match, info_regex)) {
        info.name = match[1].str();
        return true;
    }
    
    return false;
}

bool CHTLCJMODManager::parseModuleExports(const std::string& content, std::map<std::string, std::string>& exports) const {
    // Simple parsing - look for export statements
    std::regex export_regex(R"(//\s*@export\s+(\w+)\s*=\s*(.+))");
    std::sregex_iterator iter(content.begin(), content.end(), export_regex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        const std::smatch& match = *iter;
        exports[match[1].str()] = match[2].str();
    }
    
    return true;
}

bool CHTLCJMODManager::parseModuleImports(const std::string& content, std::map<std::string, std::string>& imports) const {
    // Simple parsing - look for import statements
    std::regex import_regex(R"(//\s*@import\s+(\w+)\s*from\s*(.+))");
    std::sregex_iterator iter(content.begin(), content.end(), import_regex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        const std::smatch& match = *iter;
        imports[match[1].str()] = match[2].str();
    }
    
    return true;
}

std::string CHTLCJMODManager::readModuleFile(const std::string& path) const {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool CHTLCJMODManager::isModuleFile(const std::string& path) const {
    return path.length() >= 5 && (path.substr(path.length() - 5) == ".cjjs" || path.substr(path.length() - 5) == ".cjmod");
}

std::vector<std::string> CHTLCJMODManager::findModuleFiles(const std::string& /* directory */) const {
    std::vector<std::string> files;
    // Simplified implementation - would use filesystem in real implementation
    return files;
}

bool CHTLCJMODManager::hasCircularDependency(const std::string& name, const std::set<std::string>& visited) const {
    if (visited.find(name) != visited.end()) {
        return true;
    }
    
    auto dependencies = getModuleDependencies(name);
    std::set<std::string> new_visited = visited;
    new_visited.insert(name);
    
    for (const auto& dep : dependencies) {
        if (hasCircularDependency(dep.name, new_visited)) {
            return true;
        }
    }
    
    return false;
}

// CHTLMixedModuleManager implementation
CHTLMixedModuleManager::CHTLMixedModuleManager() {
}

CHTLMixedModuleManager::~CHTLMixedModuleManager() {
}

bool CHTLMixedModuleManager::loadModule(const std::string& name, const std::string& path) {
    if (name.empty() || path.empty()) {
        return false;
    }
    
    std::string content = readModuleFile(path);
    if (content.empty()) {
        return false;
    }
    
    MixedModule module(name);
    module.path = path;
    
    // Split mixed module into CMOD and CJMOD parts
    std::string cmod_part, cjmod_part;
    if (!splitMixedModule(content, cmod_part, cjmod_part)) {
        return false;
    }
    
    module.cmod_part.content = cmod_part;
    module.cjmod_part.content = cjmod_part;
    
    // Parse module info
    if (!parseModuleInfo(content, module.info)) {
        return false;
    }
    
    // Parse exports and imports
    parseModuleExports(content, module.cmod_part.exports);
    parseModuleImports(content, module.cmod_part.imports);
    parseModuleExports(content, module.cjmod_part.exports);
    parseModuleImports(content, module.cjmod_part.imports);
    
    modules[name] = module;
    return true;
}

bool CHTLMixedModuleManager::unloadModule(const std::string& name) {
    return modules.erase(name) > 0;
}

bool CHTLMixedModuleManager::reloadModule(const std::string& name) {
    auto it = modules.find(name);
    if (it == modules.end()) {
        return false;
    }
    
    std::string path = it->second.path;
    unloadModule(name);
    return loadModule(name, path);
}

bool CHTLMixedModuleManager::hasModule(const std::string& name) const {
    return modules.find(name) != modules.end();
}

MixedModule CHTLMixedModuleManager::getModule(const std::string& name) const {
    auto it = modules.find(name);
    return (it != modules.end()) ? it->second : MixedModule();
}

std::vector<std::string> CHTLMixedModuleManager::getModuleNames() const {
    std::vector<std::string> names;
    for (const auto& pair : modules) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> CHTLMixedModuleManager::getModulePaths() const {
    std::vector<std::string> paths;
    for (const auto& pair : modules) {
        paths.push_back(pair.second.path);
    }
    return paths;
}

std::string CHTLMixedModuleManager::getModuleContent(const std::string& name) const {
    auto it = modules.find(name);
    if (it != modules.end()) {
        return it->second.cmod_part.content + "\n" + it->second.cjmod_part.content;
    }
    return "";
}

std::string CHTLMixedModuleManager::getCMODContent(const std::string& name) const {
    auto it = modules.find(name);
    return (it != modules.end()) ? it->second.cmod_part.content : "";
}

std::string CHTLMixedModuleManager::getCJMODContent(const std::string& name) const {
    auto it = modules.find(name);
    return (it != modules.end()) ? it->second.cjmod_part.content : "";
}

std::string CHTLMixedModuleManager::getModuleExport(const std::string& name, const std::string& export_name) const {
    auto it = modules.find(name);
    if (it != modules.end()) {
        // Check CMOD exports first
        auto cmod_it = it->second.cmod_part.exports.find(export_name);
        if (cmod_it != it->second.cmod_part.exports.end()) {
            return cmod_it->second;
        }
        
        // Check CJMOD exports
        auto cjmod_it = it->second.cjmod_part.exports.find(export_name);
        if (cjmod_it != it->second.cjmod_part.exports.end()) {
            return cjmod_it->second;
        }
    }
    return "";
}

std::vector<std::string> CHTLMixedModuleManager::getModuleExports(const std::string& name) const {
    std::vector<std::string> exports;
    auto it = modules.find(name);
    if (it != modules.end()) {
        for (const auto& pair : it->second.cmod_part.exports) {
            exports.push_back(pair.first);
        }
        for (const auto& pair : it->second.cjmod_part.exports) {
            exports.push_back(pair.first);
        }
    }
    return exports;
}

std::vector<std::string> CHTLMixedModuleManager::getModuleImports(const std::string& name) const {
    std::vector<std::string> imports;
    auto it = modules.find(name);
    if (it != modules.end()) {
        for (const auto& pair : it->second.cmod_part.imports) {
            imports.push_back(pair.first);
        }
        for (const auto& pair : it->second.cjmod_part.imports) {
            imports.push_back(pair.first);
        }
    }
    return imports;
}

void CHTLMixedModuleManager::addSearchPath(const std::string& path, bool recursive, int priority) {
    ModuleSearchPath search_path(path, recursive, priority);
    search_paths.push_back(search_path);
    
    // Sort by priority (higher priority first)
    std::sort(search_paths.begin(), search_paths.end(),
        [](const ModuleSearchPath& a, const ModuleSearchPath& b) {
            return a.priority > b.priority;
        });
}

void CHTLMixedModuleManager::removeSearchPath(const std::string& path) {
    search_paths.erase(
        std::remove_if(search_paths.begin(), search_paths.end(),
            [&path](const ModuleSearchPath& sp) {
                return sp.path == path;
            }),
        search_paths.end());
}

std::vector<ModuleSearchPath> CHTLMixedModuleManager::getSearchPaths() const {
    return search_paths;
}

std::vector<ModuleDependency> CHTLMixedModuleManager::getModuleDependencies(const std::string& name) const {
    std::vector<ModuleDependency> dependencies;
    auto it = modules.find(name);
    if (it != modules.end()) {
        // Parse dependencies from module info
        for (const auto& dep_name : it->second.info.dependencies) {
            ModuleDependency dep(dep_name);
            dependencies.push_back(dep);
        }
    }
    return dependencies;
}

bool CHTLMixedModuleManager::resolveDependencies(const std::string& name) {
    auto dependencies = getModuleDependencies(name);
    for (const auto& dep : dependencies) {
        if (!hasModule(dep.name)) {
            std::string dep_path = findModulePath(dep.name);
            if (dep_path.empty() || !loadModule(dep.name, dep_path)) {
                return false;
            }
        }
    }
    return true;
}

std::vector<std::string> CHTLMixedModuleManager::getDependencyChain(const std::string& name) const {
    std::vector<std::string> chain;
    std::set<std::string> visited;
    
    std::function<void(const std::string&)> buildChain = [&](const std::string& module_name) {
        if (visited.find(module_name) != visited.end()) {
            return; // Avoid circular dependencies
        }
        
        visited.insert(module_name);
        auto dependencies = getModuleDependencies(module_name);
        
        for (const auto& dep : dependencies) {
            buildChain(dep.name);
        }
        
        chain.push_back(module_name);
    };
    
    buildChain(name);
    return chain;
}

bool CHTLMixedModuleManager::validateModule(const std::string& name) const {
    auto it = modules.find(name);
    if (it == modules.end()) {
        return false;
    }
    
    // Basic validation
    return !it->second.cmod_part.content.empty() && 
           !it->second.cjmod_part.content.empty() && 
           !it->second.info.name.empty();
}

std::vector<std::string> CHTLMixedModuleManager::getModuleErrors(const std::string& name) const {
    std::vector<std::string> errors;
    auto it = modules.find(name);
    if (it == modules.end()) {
        errors.push_back("Module not found: " + name);
        return errors;
    }
    
    if (it->second.cmod_part.content.empty()) {
        errors.push_back("CMOD content is empty");
    }
    
    if (it->second.cjmod_part.content.empty()) {
        errors.push_back("CJMOD content is empty");
    }
    
    if (it->second.info.name.empty()) {
        errors.push_back("Module name is empty");
    }
    
    return errors;
}

bool CHTLMixedModuleManager::validateAllModules() const {
    for (const auto& pair : modules) {
        if (!validateModule(pair.first)) {
            return false;
        }
    }
    return true;
}

std::string CHTLMixedModuleManager::findModulePath(const std::string& name) const {
    for (const auto& search_path : search_paths) {
        std::string full_path = search_path.path + "/" + name;
        if (isModuleFile(full_path)) {
            return full_path;
        }
        
        if (search_path.recursive) {
            auto files = findModuleFiles(search_path.path);
            for (const auto& file : files) {
                if (file.find(name) != std::string::npos) {
                    return file;
                }
            }
        }
    }
    return "";
}

std::string CHTLMixedModuleManager::generateModuleId(const std::string& name) const {
    return "mixed_" + name;
}

void CHTLMixedModuleManager::clearCache() {
    modules.clear();
}

bool CHTLMixedModuleManager::parseModuleInfo(const std::string& content, ModuleInfo& info) const {
    // Simple parsing - look for module info comments
    std::regex info_regex(R"(//\s*@module\s+(\w+))");
    std::smatch match;
    
    if (std::regex_search(content, match, info_regex)) {
        info.name = match[1].str();
        return true;
    }
    
    return false;
}

bool CHTLMixedModuleManager::parseModuleExports(const std::string& content, std::map<std::string, std::string>& exports) const {
    // Simple parsing - look for export statements
    std::regex export_regex(R"(//\s*@export\s+(\w+)\s*=\s*(.+))");
    std::sregex_iterator iter(content.begin(), content.end(), export_regex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        const std::smatch& match = *iter;
        exports[match[1].str()] = match[2].str();
    }
    
    return true;
}

bool CHTLMixedModuleManager::parseModuleImports(const std::string& content, std::map<std::string, std::string>& imports) const {
    // Simple parsing - look for import statements
    std::regex import_regex(R"(//\s*@import\s+(\w+)\s*from\s*(.+))");
    std::sregex_iterator iter(content.begin(), content.end(), import_regex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        const std::smatch& match = *iter;
        imports[match[1].str()] = match[2].str();
    }
    
    return true;
}

std::string CHTLMixedModuleManager::readModuleFile(const std::string& path) const {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool CHTLMixedModuleManager::isModuleFile(const std::string& path) const {
    return path.length() >= 5 && (path.substr(path.length() - 5) == ".chtl" || 
                                  path.substr(path.length() - 5) == ".cjjs" || 
                                  path.substr(path.length() - 6) == ".mixed");
}

std::vector<std::string> CHTLMixedModuleManager::findModuleFiles(const std::string& /* directory */) const {
    std::vector<std::string> files;
    // Simplified implementation - would use filesystem in real implementation
    return files;
}

bool CHTLMixedModuleManager::hasCircularDependency(const std::string& name, const std::set<std::string>& visited) const {
    if (visited.find(name) != visited.end()) {
        return true;
    }
    
    auto dependencies = getModuleDependencies(name);
    std::set<std::string> new_visited = visited;
    new_visited.insert(name);
    
    for (const auto& dep : dependencies) {
        if (hasCircularDependency(dep.name, new_visited)) {
            return true;
        }
    }
    
    return false;
}

bool CHTLMixedModuleManager::splitMixedModule(const std::string& content, std::string& cmod_part, std::string& cjmod_part) const {
    // Simple splitting - look for markers
    size_t cmod_start = content.find("// CMOD START");
    size_t cmod_end = content.find("// CMOD END");
    size_t cjmod_start = content.find("// CJMOD START");
    size_t cjmod_end = content.find("// CJMOD END");
    
    if (cmod_start != std::string::npos && cmod_end != std::string::npos) {
        cmod_part = content.substr(cmod_start, cmod_end - cmod_start);
    }
    
    if (cjmod_start != std::string::npos && cjmod_end != std::string::npos) {
        cjmod_part = content.substr(cjmod_start, cjmod_end - cjmod_start);
    }
    
    return !cmod_part.empty() && !cjmod_part.empty();
}

// CHTLModuleManager implementation
CHTLModuleManager::CHTLModuleManager() {
}

CHTLModuleManager::~CHTLModuleManager() {
}

bool CHTLModuleManager::loadModule(const std::string& name, const std::string& path, ModuleType type) {
    switch (type) {
        case ModuleType::CMOD:
            return cmod_manager.loadModule(name, path);
        case ModuleType::CJMOD:
            return cjmod_manager.loadModule(name, path);
        case ModuleType::Mixed:
            return mixed_manager.loadModule(name, path);
        default:
            return false;
    }
}

bool CHTLModuleManager::unloadModule(const std::string& name) {
    bool unloaded = false;
    
    if (cmod_manager.hasModule(name)) {
        unloaded = cmod_manager.unloadModule(name) || unloaded;
    }
    
    if (cjmod_manager.hasModule(name)) {
        unloaded = cjmod_manager.unloadModule(name) || unloaded;
    }
    
    if (mixed_manager.hasModule(name)) {
        unloaded = mixed_manager.unloadModule(name) || unloaded;
    }
    
    return unloaded;
}

bool CHTLModuleManager::reloadModule(const std::string& name) {
    bool reloaded = false;
    
    if (cmod_manager.hasModule(name)) {
        reloaded = cmod_manager.reloadModule(name) || reloaded;
    }
    
    if (cjmod_manager.hasModule(name)) {
        reloaded = cjmod_manager.reloadModule(name) || reloaded;
    }
    
    if (mixed_manager.hasModule(name)) {
        reloaded = mixed_manager.reloadModule(name) || reloaded;
    }
    
    return reloaded;
}

bool CHTLModuleManager::hasModule(const std::string& name) const {
    return cmod_manager.hasModule(name) || 
           cjmod_manager.hasModule(name) || 
           mixed_manager.hasModule(name);
}

ModuleType CHTLModuleManager::getModuleType(const std::string& name) const {
    if (cmod_manager.hasModule(name)) {
        return ModuleType::CMOD;
    } else if (cjmod_manager.hasModule(name)) {
        return ModuleType::CJMOD;
    } else if (mixed_manager.hasModule(name)) {
        return ModuleType::Mixed;
    }
    return ModuleType::CMOD; // Default
}

std::vector<std::string> CHTLModuleManager::getModuleNames() const {
    std::vector<std::string> names;
    
    auto cmod_names = cmod_manager.getModuleNames();
    auto cjmod_names = cjmod_manager.getModuleNames();
    auto mixed_names = mixed_manager.getModuleNames();
    
    names.insert(names.end(), cmod_names.begin(), cmod_names.end());
    names.insert(names.end(), cjmod_names.begin(), cjmod_names.end());
    names.insert(names.end(), mixed_names.begin(), mixed_names.end());
    
    return names;
}

std::vector<std::string> CHTLModuleManager::getModuleNames(ModuleType type) const {
    switch (type) {
        case ModuleType::CMOD:
            return cmod_manager.getModuleNames();
        case ModuleType::CJMOD:
            return cjmod_manager.getModuleNames();
        case ModuleType::Mixed:
            return mixed_manager.getModuleNames();
        default:
            return {};
    }
}

std::string CHTLModuleManager::getModuleContent(const std::string& name) const {
    if (cmod_manager.hasModule(name)) {
        return cmod_manager.getModuleContent(name);
    } else if (cjmod_manager.hasModule(name)) {
        return cjmod_manager.getModuleContent(name);
    } else if (mixed_manager.hasModule(name)) {
        return mixed_manager.getModuleContent(name);
    }
    return "";
}

std::string CHTLModuleManager::getModuleExport(const std::string& name, const std::string& export_name) const {
    if (cmod_manager.hasModule(name)) {
        return cmod_manager.getModuleExport(name, export_name);
    } else if (cjmod_manager.hasModule(name)) {
        return cjmod_manager.getModuleExport(name, export_name);
    } else if (mixed_manager.hasModule(name)) {
        return mixed_manager.getModuleExport(name, export_name);
    }
    return "";
}

std::vector<std::string> CHTLModuleManager::getModuleExports(const std::string& name) const {
    if (cmod_manager.hasModule(name)) {
        return cmod_manager.getModuleExports(name);
    } else if (cjmod_manager.hasModule(name)) {
        return cjmod_manager.getModuleExports(name);
    } else if (mixed_manager.hasModule(name)) {
        return mixed_manager.getModuleExports(name);
    }
    return {};
}

std::vector<std::string> CHTLModuleManager::getModuleImports(const std::string& name) const {
    if (cmod_manager.hasModule(name)) {
        return cmod_manager.getModuleImports(name);
    } else if (cjmod_manager.hasModule(name)) {
        return cjmod_manager.getModuleImports(name);
    } else if (mixed_manager.hasModule(name)) {
        return mixed_manager.getModuleImports(name);
    }
    return {};
}

void CHTLModuleManager::addSearchPath(const std::string& path, bool recursive, int priority) {
    cmod_manager.addSearchPath(path, recursive, priority);
    cjmod_manager.addSearchPath(path, recursive, priority);
    mixed_manager.addSearchPath(path, recursive, priority);
}

void CHTLModuleManager::removeSearchPath(const std::string& path) {
    cmod_manager.removeSearchPath(path);
    cjmod_manager.removeSearchPath(path);
    mixed_manager.removeSearchPath(path);
}

std::vector<ModuleSearchPath> CHTLModuleManager::getSearchPaths() const {
    return cmod_manager.getSearchPaths();
}

std::vector<ModuleDependency> CHTLModuleManager::getModuleDependencies(const std::string& name) const {
    if (cmod_manager.hasModule(name)) {
        return cmod_manager.getModuleDependencies(name);
    } else if (cjmod_manager.hasModule(name)) {
        return cjmod_manager.getModuleDependencies(name);
    } else if (mixed_manager.hasModule(name)) {
        return mixed_manager.getModuleDependencies(name);
    }
    return {};
}

bool CHTLModuleManager::resolveDependencies(const std::string& name) {
    if (cmod_manager.hasModule(name)) {
        return cmod_manager.resolveDependencies(name);
    } else if (cjmod_manager.hasModule(name)) {
        return cjmod_manager.resolveDependencies(name);
    } else if (mixed_manager.hasModule(name)) {
        return mixed_manager.resolveDependencies(name);
    }
    return false;
}

std::vector<std::string> CHTLModuleManager::getDependencyChain(const std::string& name) const {
    if (cmod_manager.hasModule(name)) {
        return cmod_manager.getDependencyChain(name);
    } else if (cjmod_manager.hasModule(name)) {
        return cjmod_manager.getDependencyChain(name);
    } else if (mixed_manager.hasModule(name)) {
        return mixed_manager.getDependencyChain(name);
    }
    return {};
}

bool CHTLModuleManager::validateModule(const std::string& name) const {
    if (cmod_manager.hasModule(name)) {
        return cmod_manager.validateModule(name);
    } else if (cjmod_manager.hasModule(name)) {
        return cjmod_manager.validateModule(name);
    } else if (mixed_manager.hasModule(name)) {
        return mixed_manager.validateModule(name);
    }
    return false;
}

std::vector<std::string> CHTLModuleManager::getModuleErrors(const std::string& name) const {
    if (cmod_manager.hasModule(name)) {
        return cmod_manager.getModuleErrors(name);
    } else if (cjmod_manager.hasModule(name)) {
        return cjmod_manager.getModuleErrors(name);
    } else if (mixed_manager.hasModule(name)) {
        return mixed_manager.getModuleErrors(name);
    }
    return {"Module not found: " + name};
}

bool CHTLModuleManager::validateAllModules() const {
    return cmod_manager.validateAllModules() && 
           cjmod_manager.validateAllModules() && 
           mixed_manager.validateAllModules();
}

std::string CHTLModuleManager::findModulePath(const std::string& name) const {
    std::string path = cmod_manager.findModulePath(name);
    if (!path.empty()) return path;
    
    path = cjmod_manager.findModulePath(name);
    if (!path.empty()) return path;
    
    path = mixed_manager.findModulePath(name);
    return path;
}

std::string CHTLModuleManager::generateModuleId(const std::string& name) const {
    ModuleType type = getModuleType(name);
    switch (type) {
        case ModuleType::CMOD:
            return cmod_manager.generateModuleId(name);
        case ModuleType::CJMOD:
            return cjmod_manager.generateModuleId(name);
        case ModuleType::Mixed:
            return mixed_manager.generateModuleId(name);
        default:
            return "module_" + name;
    }
}

void CHTLModuleManager::clearCache() {
    cmod_manager.clearCache();
    cjmod_manager.clearCache();
    mixed_manager.clearCache();
}

CHTLCMODManager& CHTLModuleManager::getCMODManager() {
    return cmod_manager;
}

CHTLCJMODManager& CHTLModuleManager::getCJMODManager() {
    return cjmod_manager;
}

CHTLMixedModuleManager& CHTLModuleManager::getMixedModuleManager() {
    return mixed_manager;
}

ModuleType CHTLModuleManager::detectModuleType(const std::string& content) const {
    // Simple detection based on content markers
    if (content.find("// CMOD START") != std::string::npos && 
        content.find("// CJMOD START") != std::string::npos) {
        return ModuleType::Mixed;
    } else if (content.find(".cjjs") != std::string::npos || 
               content.find("fileloader") != std::string::npos) {
        return ModuleType::CJMOD;
    } else {
        return ModuleType::CMOD;
    }
}

std::string CHTLModuleManager::getModuleManagerName(const std::string& name) const {
    if (cmod_manager.hasModule(name)) {
        return "cmod";
    } else if (cjmod_manager.hasModule(name)) {
        return "cjmod";
    } else if (mixed_manager.hasModule(name)) {
        return "mixed";
    }
    return "";
}

} // namespace CHTL