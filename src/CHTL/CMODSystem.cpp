#include "CMODSystem.h"
#include "CHTLLoader.h"
#include <fstream>
#include <sstream>

namespace CHTL {

CMODSystem::CMODSystem() {
}

bool CMODSystem::loadModule(const std::string& modulePath) {
    try {
        CHTLLoader loader;
        NodePtr moduleContent = loader.loadFile(modulePath);
        
        if (!moduleContent) {
            reportError("Failed to load module: " + modulePath);
            return false;
        }
        
        // 提取模块名称
        std::string moduleName = extractModuleName(modulePath);
        
        // 加载模块信息
        CMODInfo info;
        if (!parseModuleInfo(moduleContent, info)) {
            reportError("Failed to parse module info: " + modulePath);
            return false;
        }
        
        loadedModules_[moduleName] = moduleContent;
        moduleInfos_[moduleName] = info;
        
        return true;
    } catch (const std::exception& e) {
        reportError("Error loading module: " + std::string(e.what()));
        return false;
    }
}

bool CMODSystem::unloadModule(const std::string& moduleName) {
    auto it = loadedModules_.find(moduleName);
    if (it != loadedModules_.end()) {
        loadedModules_.erase(it);
        moduleInfos_.erase(moduleName);
        return true;
    }
    return false;
}

bool CMODSystem::isModuleLoaded(const std::string& moduleName) const {
    return loadedModules_.find(moduleName) != loadedModules_.end();
}

CMODInfo CMODSystem::getModuleInfo(const std::string& moduleName) const {
    auto it = moduleInfos_.find(moduleName);
    return (it != moduleInfos_.end()) ? it->second : CMODInfo();
}

std::vector<std::string> CMODSystem::getLoadedModules() const {
    std::vector<std::string> modules;
    for (const auto& pair : loadedModules_) {
        modules.push_back(pair.first);
    }
    return modules;
}

NodePtr CMODSystem::getModuleContent(const std::string& moduleName) const {
    auto it = loadedModules_.find(moduleName);
    return (it != loadedModules_.end()) ? it->second : nullptr;
}

void CMODSystem::setErrorHandler(std::function<void(const std::string&)> handler) {
    errorHandler_ = handler;
}

void CMODSystem::reportError(const std::string& message) {
    if (errorHandler_) {
        errorHandler_(message);
    }
}

bool CMODSystem::parseModuleInfo(const std::string& content, CMODInfo& info) {
    // 简单的模块信息解析
    // 这里应该实现更复杂的解析逻辑
    info.name = "Unknown";
    info.version = "1.0.0";
    info.description = "CHTL Module";
    info.author = "Unknown";
    info.license = "MIT";
    info.dependencies = "";
    info.category = "general";
    info.minCHTLVersion = "1.0.0";
    info.maxCHTLVersion = "2.0.0";
    
    return true;
}

std::string CMODSystem::extractModuleName(const std::string& modulePath) {
    size_t lastSlash = modulePath.find_last_of("/\\");
    std::string filename = (lastSlash != std::string::npos) ? 
                          modulePath.substr(lastSlash + 1) : modulePath;
    
    size_t lastDot = filename.find_last_of(".");
    return (lastDot != std::string::npos) ? 
           filename.substr(0, lastDot) : filename;
}

} // namespace CHTL