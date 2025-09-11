#include "CJMODSystem.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace CHTL_JS {

CJMODSystem::CJMODSystem() : debugMode(false) {
}

CJMODSystem::CJMODSystem(const std::string& path) : modulePath(path), debugMode(false) {
}

bool CJMODSystem::loadModule(const std::string& moduleName) {
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

bool CJMODSystem::unloadModule(const std::string& moduleName) {
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

bool CJMODSystem::isModuleLoaded(const std::string& moduleName) const {
    return std::find(loadedModules.begin(), loadedModules.end(), moduleName) != loadedModules.end();
}

void CJMODSystem::cacheModule(const std::string& moduleName, const std::string& content) {
    moduleCache[moduleName] = content;
}

std::string CJMODSystem::getCachedModule(const std::string& moduleName) const {
    auto it = moduleCache.find(moduleName);
    return it != moduleCache.end() ? it->second : "";
}

bool CJMODSystem::isModuleCached(const std::string& moduleName) const {
    return moduleCache.find(moduleName) != moduleCache.end();
}

void CJMODSystem::clearCache() {
    moduleCache.clear();
}

void CJMODSystem::clearErrors() {
    errors.clear();
}

std::string CJMODSystem::resolveModulePath(const std::string& moduleName) const {
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

bool CJMODSystem::moduleExists(const std::string& moduleName) const {
    std::string fullPath = resolveModulePath(moduleName);
    return std::filesystem::exists(fullPath);
}

std::string CJMODSystem::getModuleContent(const std::string& moduleName) const {
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

std::string CJMODSystem::toString() const {
    std::ostringstream oss;
    oss << "CJMODSystem{";
    oss << "modulePath=" << modulePath << ", ";
    oss << "loadedModules=" << loadedModules.size() << ", ";
    oss << "cachedModules=" << moduleCache.size() << ", ";
    oss << "errors=" << errors.size() << ", ";
    oss << "debugMode=" << (debugMode ? "true" : "false");
    oss << "}";
    return oss.str();
}

std::string CJMODSystem::toDebugString() const {
    std::ostringstream oss;
    oss << "CJMODSystem Debug Info:\n";
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

std::string CJMODSystem::toJSON() const {
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

std::string CJMODSystem::toXML() const {
    std::ostringstream oss;
    oss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    oss << "<CJMODSystem>\n";
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
    oss << "</CJMODSystem>";
    return oss.str();
}

std::string CJMODSystem::toYAML() const {
    std::ostringstream oss;
    oss << "CJMODSystem:\n";
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

CJMODSystem CJMODSystem::clone() const {
    CJMODSystem cloned;
    cloned.modulePath = modulePath;
    cloned.loadedModules = loadedModules;
    cloned.moduleCache = moduleCache;
    cloned.errors = errors;
    cloned.debugMode = debugMode;
    return cloned;
}

bool CJMODSystem::operator==(const CJMODSystem& other) const {
    return modulePath == other.modulePath &&
           loadedModules == other.loadedModules &&
           moduleCache == other.moduleCache &&
           errors == other.errors &&
           debugMode == other.debugMode;
}

bool CJMODSystem::operator!=(const CJMODSystem& other) const {
    return !(*this == other);
}

} // namespace CHTL_JS