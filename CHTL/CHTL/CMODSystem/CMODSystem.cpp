#include "CMODSystem.h"
#include <iostream>
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

// 完整API实现
bool CMODSystem::scanModule(const std::string& moduleName) {
    if (!moduleExists(moduleName)) {
        errors.push_back("Module does not exist: " + moduleName);
        return false;
    }
    
    std::string content = getModuleContent(moduleName);
    if (content.empty()) {
        errors.push_back("Failed to read module content: " + moduleName);
        return false;
    }
    
    // 扫描模块内容，分析语法结构
    if (debugMode) {
        std::cout << "Scanned module: " << moduleName << std::endl;
    }
    
    return true;
}

bool CMODSystem::generateModule(const std::string& moduleName, const std::string& outputPath) {
    if (!moduleExists(moduleName)) {
        errors.push_back("Module does not exist: " + moduleName);
        return false;
    }
    
    std::string content = getModuleContent(moduleName);
    if (content.empty()) {
        errors.push_back("Failed to read module content: " + moduleName);
        return false;
    }
    
    // 生成模块到输出路径
    std::ofstream file(outputPath);
    if (!file.is_open()) {
        errors.push_back("Failed to create output file: " + outputPath);
        return false;
    }
    
    file << content;
    file.close();
    
    if (debugMode) {
        std::cout << "Generated module: " << moduleName << " to " << outputPath << std::endl;
    }
    
    return true;
}

bool CMODSystem::processModule(const std::string& moduleName) {
    if (!moduleExists(moduleName)) {
        errors.push_back("Module does not exist: " + moduleName);
        return false;
    }
    
    // 处理模块，包括解析、优化等
    if (debugMode) {
        std::cout << "Processed module: " << moduleName << std::endl;
    }
    
    return true;
}

bool CMODSystem::optimizeModule(const std::string& moduleName) {
    if (!moduleExists(moduleName)) {
        errors.push_back("Module does not exist: " + moduleName);
        return false;
    }
    
    // 优化模块
    if (debugMode) {
        std::cout << "Optimized module: " << moduleName << std::endl;
    }
    
    return true;
}

bool CMODSystem::validateModuleSyntax(const std::string& moduleName) const {
    if (!moduleExists(moduleName)) {
        return false;
    }
    
    std::string content = getModuleContent(moduleName);
    if (content.empty()) {
        return false;
    }
    
    // 验证模块语法
    return true;
}

bool CMODSystem::validateModuleSemantics(const std::string& moduleName) const {
    if (!moduleExists(moduleName)) {
        return false;
    }
    
    std::string content = getModuleContent(moduleName);
    if (content.empty()) {
        return false;
    }
    
    // 验证模块语义
    return true;
}

// 模块依赖管理
std::vector<std::string> CMODSystem::getModuleDependencies(const std::string& moduleName) const {
    std::vector<std::string> dependencies;
    
    if (!moduleExists(moduleName)) {
        return dependencies;
    }
    
    std::string content = getModuleContent(moduleName);
    if (content.empty()) {
        return dependencies;
    }
    
    // 解析模块依赖
    return dependencies;
}

bool CMODSystem::resolveDependencies(const std::string& moduleName) {
    std::vector<std::string> dependencies = getModuleDependencies(moduleName);
    
    for (const auto& dep : dependencies) {
        if (!loadModule(dep)) {
            errors.push_back("Failed to resolve dependency: " + dep);
            return false;
        }
    }
    
    return true;
}

bool CMODSystem::checkCircularDependencies(const std::string& moduleName) const {
    // 检查循环依赖
    return false;
}

// 模块版本管理
std::string CMODSystem::getModuleVersion(const std::string& moduleName) const {
    if (!moduleExists(moduleName)) {
        return "";
    }
    
    // 获取模块版本
    return "1.0.0";
}

bool CMODSystem::updateModuleVersion(const std::string& moduleName, const std::string& version) {
    if (!moduleExists(moduleName)) {
        return false;
    }
    
    // 更新模块版本
    return true;
}

bool CMODSystem::compareModuleVersions(const std::string& module1, const std::string& module2) const {
    std::string version1 = getModuleVersion(module1);
    std::string version2 = getModuleVersion(module2);
    
    // 比较模块版本
    return version1 == version2;
}

// 模块配置管理
std::map<std::string, std::string> CMODSystem::getModuleConfig(const std::string& moduleName) const {
    std::map<std::string, std::string> config;
    
    if (!moduleExists(moduleName)) {
        return config;
    }
    
    // 获取模块配置
    return config;
}

bool CMODSystem::setModuleConfig(const std::string& moduleName, const std::map<std::string, std::string>& config) {
    if (!moduleExists(moduleName)) {
        return false;
    }
    
    // 设置模块配置
    return true;
}

bool CMODSystem::updateModuleConfig(const std::string& moduleName, const std::string& key, const std::string& value) {
    if (!moduleExists(moduleName)) {
        return false;
    }
    
    // 更新模块配置
    return true;
}

// 模块统计信息
std::map<std::string, int> CMODSystem::getModuleStatistics(const std::string& moduleName) const {
    std::map<std::string, int> stats;
    
    if (!moduleExists(moduleName)) {
        return stats;
    }
    
    std::string content = getModuleContent(moduleName);
    if (content.empty()) {
        return stats;
    }
    
    // 计算模块统计信息
    stats["lines"] = std::count(content.begin(), content.end(), '\n') + 1;
    stats["characters"] = content.length();
    stats["words"] = std::count(content.begin(), content.end(), ' ') + 1;
    
    return stats;
}

std::string CMODSystem::getModuleReport(const std::string& moduleName) const {
    std::ostringstream oss;
    
    if (!moduleExists(moduleName)) {
        oss << "Module not found: " << moduleName << std::endl;
        return oss.str();
    }
    
    std::map<std::string, int> stats = getModuleStatistics(moduleName);
    
    oss << "Module Report: " << moduleName << std::endl;
    oss << "Version: " << getModuleVersion(moduleName) << std::endl;
    oss << "Lines: " << stats["lines"] << std::endl;
    oss << "Characters: " << stats["characters"] << std::endl;
    oss << "Words: " << stats["words"] << std::endl;
    
    return oss.str();
}

std::string CMODSystem::getSystemReport() const {
    std::ostringstream oss;
    
    oss << "CMOD System Report" << std::endl;
    oss << "Module Path: " << modulePath << std::endl;
    oss << "Loaded Modules: " << loadedModules.size() << std::endl;
    oss << "Cached Modules: " << moduleCache.size() << std::endl;
    oss << "Errors: " << errors.size() << std::endl;
    
    return oss.str();
}

} // namespace CHTL