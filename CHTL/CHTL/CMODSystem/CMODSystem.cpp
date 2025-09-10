#include "CMODSystem.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace CHTL {

CMODSystem::CMODSystem() : debugMode(false) {
}

CMODSystem::CMODSystem(const std::string& path) : modulePath(path), debugMode(false) {
}

bool CMODSystem::loadModule(const std::string& moduleName) {
    if (isModuleLoaded(moduleName)) {
        return true;
    }
    
    std::string content = getModuleContent(moduleName);
    if (content.empty()) {
        errors.push_back("Failed to load module: " + moduleName);
        return false;
    }
    
    loadedModules.push_back(moduleName);
    cacheModule(moduleName, content);
    
    if (debugMode) {
        std::cout << "Loaded module: " << moduleName << std::endl;
    }
    
    return true;
}

bool CMODSystem::unloadModule(const std::string& moduleName) {
    auto it = std::find(loadedModules.begin(), loadedModules.end(), moduleName);
    if (it == loadedModules.end()) {
        return false;
    }
    
    loadedModules.erase(it);
    moduleCache.erase(moduleName);
    
    if (debugMode) {
        std::cout << "Unloaded module: " << moduleName << std::endl;
    }
    
    return true;
}

bool CMODSystem::isModuleLoaded(const std::string& moduleName) const {
    return std::find(loadedModules.begin(), loadedModules.end(), moduleName) != loadedModules.end();
}

void CMODSystem::cacheModule(const std::string& moduleName, const std::string& content) {
    moduleCache[moduleName] = content;
}

std::string CMODSystem::getCachedModule(const std::string& moduleName) const {
    auto it = moduleCache.find(moduleName);
    return it != moduleCache.end() ? it->second : "";
}

bool CMODSystem::isModuleCached(const std::string& moduleName) const {
    return moduleCache.find(moduleName) != moduleCache.end();
}

void CMODSystem::clearCache() {
    moduleCache.clear();
}

void CMODSystem::clearErrors() {
    errors.clear();
}

std::string CMODSystem::resolveModulePath(const std::string& moduleName) const {
    if (modulePath.empty()) {
        return moduleName;
    }
    
    std::filesystem::path base(modulePath);
    std::filesystem::path module(moduleName);
    
    if (module.is_absolute()) {
        return moduleName;
    }
    
    return (base / module).string();
}

bool CMODSystem::moduleExists(const std::string& moduleName) const {
    std::string fullPath = resolveModulePath(moduleName);
    return std::filesystem::exists(fullPath);
}

std::string CMODSystem::getModuleContent(const std::string& moduleName) const {
    if (isModuleCached(moduleName)) {
        return getCachedModule(moduleName);
    }
    
    std::string fullPath = resolveModulePath(moduleName);
    
    if (!std::filesystem::exists(fullPath)) {
        if (debugMode) {
            std::cerr << "Module file not found: " << fullPath << std::endl;
        }
        return "";
    }
    
    std::ifstream file(fullPath);
    if (!file.is_open()) {
        if (debugMode) {
            std::cerr << "Cannot open module file: " << fullPath << std::endl;
        }
        return "";
    }
    
    std::ostringstream content;
    content << file.rdbuf();
    file.close();
    
    return content.str();
}

std::string CMODSystem::toString() const {
    std::ostringstream oss;
    oss << "CMODSystem{";
    oss << "modulePath=" << modulePath << ", ";
    oss << "loadedModules=" << loadedModules.size() << ", ";
    oss << "cachedModules=" << moduleCache.size() << ", ";
    oss << "errors=" << errors.size() << ", ";
    oss << "debugMode=" << (debugMode ? "true" : "false");
    oss << "}";
    return oss.str();
}

std::string CMODSystem::toDebugString() const {
    std::ostringstream oss;
    oss << "CMODSystem Debug Info:\n";
    oss << "  Module Path: " << modulePath << "\n";
    oss << "  Loaded Modules: " << loadedModules.size() << "\n";
    for (const auto& module : loadedModules) {
        oss << "    " << module << "\n";
    }
    oss << "  Cached Modules: " << moduleCache.size() << "\n";
    for (const auto& module : moduleCache) {
        oss << "    " << module.first << "\n";
    }
    oss << "  Errors: " << errors.size() << "\n";
    for (const auto& error : errors) {
        oss << "    " << error << "\n";
    }
    oss << "  Debug Mode: " << (debugMode ? "true" : "false") << "\n";
    return oss.str();
}

std::string CMODSystem::toJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"modulePath\": \"" << modulePath << "\",\n";
    oss << "  \"loadedModules\": [\n";
    bool first = true;
    for (const auto& module : loadedModules) {
        if (!first) oss << ",\n";
        oss << "    \"" << module << "\"";
        first = false;
    }
    oss << "\n  ],\n";
    oss << "  \"cachedModules\": [\n";
    first = true;
    for (const auto& module : moduleCache) {
        if (!first) oss << ",\n";
        oss << "    \"" << module.first << "\"";
        first = false;
    }
    oss << "\n  ],\n";
    oss << "  \"errors\": [\n";
    first = true;
    for (const auto& error : errors) {
        if (!first) oss << ",\n";
        oss << "    \"" << error << "\"";
        first = false;
    }
    oss << "\n  ],\n";
    oss << "  \"debugMode\": " << (debugMode ? "true" : "false") << "\n";
    oss << "}";
    return oss.str();
}

std::string CMODSystem::toXML() const {
    std::ostringstream oss;
    oss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    oss << "<CMODSystem>\n";
    oss << "  <modulePath>" << modulePath << "</modulePath>\n";
    oss << "  <loadedModules>\n";
    for (const auto& module : loadedModules) {
        oss << "    <module>" << module << "</module>\n";
    }
    oss << "  </loadedModules>\n";
    oss << "  <cachedModules>\n";
    for (const auto& module : moduleCache) {
        oss << "    <module name=\"" << module.first << "\"/>\n";
    }
    oss << "  </cachedModules>\n";
    oss << "  <errors>\n";
    for (const auto& error : errors) {
        oss << "    <error>" << error << "</error>\n";
    }
    oss << "  </errors>\n";
    oss << "  <debugMode>" << (debugMode ? "true" : "false") << "</debugMode>\n";
    oss << "</CMODSystem>";
    return oss.str();
}

std::string CMODSystem::toYAML() const {
    std::ostringstream oss;
    oss << "CMODSystem:\n";
    oss << "  modulePath: " << modulePath << "\n";
    oss << "  loadedModules:\n";
    for (const auto& module : loadedModules) {
        oss << "    - " << module << "\n";
    }
    oss << "  cachedModules:\n";
    for (const auto& module : moduleCache) {
        oss << "    - " << module.first << "\n";
    }
    oss << "  errors:\n";
    for (const auto& error : errors) {
        oss << "    - " << error << "\n";
    }
    oss << "  debugMode: " << (debugMode ? "true" : "false") << "\n";
    return oss.str();
}

CMODSystem CMODSystem::clone() const {
    CMODSystem cloned;
    cloned.modulePath = modulePath;
    cloned.loadedModules = loadedModules;
    cloned.moduleCache = moduleCache;
    cloned.errors = errors;
    cloned.debugMode = debugMode;
    return cloned;
}

bool CMODSystem::operator==(const CMODSystem& other) const {
    return modulePath == other.modulePath &&
           loadedModules == other.loadedModules &&
           moduleCache == other.moduleCache &&
           errors == other.errors &&
           debugMode == other.debugMode;
}

bool CMODSystem::operator!=(const CMODSystem& other) const {
    return !(*this == other);
}

} // namespace CHTL