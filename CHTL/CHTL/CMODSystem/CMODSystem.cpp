#include "CMODSystem.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>

namespace CHTL {

CMODSystem::CMODSystem() 
    : modulePath("./modules"), buildPath("./build"), debugMode(false) {}

CMODSystem::CMODSystem(const std::string& modulePath, const std::string& buildPath)
    : modulePath(modulePath), buildPath(buildPath), debugMode(false) {}

bool CMODSystem::validateModule(const CMODInfo& moduleInfo) const {
    if (moduleInfo.name.empty()) {
        return false;
    }
    
    if (moduleInfo.version.empty()) {
        return false;
    }
    
    if (moduleInfo.type == CMODType::UNKNOWN) {
        return false;
    }
    
    return true;
}

bool CMODSystem::validateDependency(const CMODDependency& dependency) const {
    if (dependency.name.empty()) {
        return false;
    }
    
    if (dependency.source.empty()) {
        return false;
    }
    
    return true;
}

bool CMODSystem::validateExport(const CMODExport& exportItem) const {
    if (exportItem.name.empty()) {
        return false;
    }
    
    if (exportItem.type.empty()) {
        return false;
    }
    
    if (!exportItem.content) {
        return false;
    }
    
    return true;
}

bool CMODSystem::validateImport(const CMODImport& importItem) const {
    if (importItem.name.empty()) {
        return false;
    }
    
    if (importItem.source.empty()) {
        return false;
    }
    
    return true;
}

std::string CMODSystem::generateModuleId(const std::string& name, const std::string& version) const {
    return name + "@" + version;
}

std::string CMODSystem::getModulePath(const std::string& moduleName) const {
    return modulePath + "/" + moduleName;
}

std::string CMODSystem::getBuildPath(const std::string& moduleName) const {
    return buildPath + "/" + moduleName;
}

bool CMODSystem::loadModuleInfo(const std::string& modulePath, CMODInfo& moduleInfo) {
    std::string infoPath = modulePath + "/info/module.json";
    std::ifstream file(infoPath);
    
    if (!file.is_open()) {
        return false;
    }
    
    // 简化的JSON解析（实际项目中应使用JSON库）
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("\"name\"") != std::string::npos) {
            size_t start = line.find("\"") + 1;
            size_t end = line.find("\"", start);
            if (end != std::string::npos) {
                moduleInfo.name = line.substr(start, end - start);
            }
        } else if (line.find("\"version\"") != std::string::npos) {
            size_t start = line.find("\"") + 1;
            size_t end = line.find("\"", start);
            if (end != std::string::npos) {
                moduleInfo.version = line.substr(start, end - start);
            }
        } else if (line.find("\"description\"") != std::string::npos) {
            size_t start = line.find("\"") + 1;
            size_t end = line.find("\"", start);
            if (end != std::string::npos) {
                moduleInfo.description = line.substr(start, end - start);
            }
        } else if (line.find("\"author\"") != std::string::npos) {
            size_t start = line.find("\"") + 1;
            size_t end = line.find("\"", start);
            if (end != std::string::npos) {
                moduleInfo.author = line.substr(start, end - start);
            }
        } else if (line.find("\"license\"") != std::string::npos) {
            size_t start = line.find("\"") + 1;
            size_t end = line.find("\"", start);
            if (end != std::string::npos) {
                moduleInfo.license = line.substr(start, end - start);
            }
        } else if (line.find("\"main\"") != std::string::npos) {
            size_t start = line.find("\"") + 1;
            size_t end = line.find("\"", start);
            if (end != std::string::npos) {
                moduleInfo.mainFile = line.substr(start, end - start);
            }
        }
    }
    
    file.close();
    return true;
}

bool CMODSystem::saveModuleInfo(const std::string& modulePath, const CMODInfo& moduleInfo) {
    std::string infoPath = modulePath + "/info/module.json";
    
    // 确保目录存在
    std::filesystem::create_directories(modulePath + "/info");
    
    std::ofstream file(infoPath);
    if (!file.is_open()) {
        return false;
    }
    
    // 简化的JSON生成（实际项目中应使用JSON库）
    file << "{\n";
    file << "  \"name\": \"" << moduleInfo.name << "\",\n";
    file << "  \"version\": \"" << moduleInfo.version << "\",\n";
    file << "  \"description\": \"" << moduleInfo.description << "\",\n";
    file << "  \"author\": \"" << moduleInfo.author << "\",\n";
    file << "  \"license\": \"" << moduleInfo.license << "\",\n";
    file << "  \"main\": \"" << moduleInfo.mainFile << "\",\n";
    file << "  \"type\": \"" << static_cast<int>(moduleInfo.type) << "\"\n";
    file << "}\n";
    
    file.close();
    return true;
}

bool CMODSystem::loadModuleSource(const std::string& modulePath, std::string& source) {
    std::string sourcePath = modulePath + "/src/main.chtl";
    std::ifstream file(sourcePath);
    
    if (!file.is_open()) {
        return false;
    }
    
    std::ostringstream oss;
    oss << file.rdbuf();
    source = oss.str();
    
    file.close();
    return true;
}

bool CMODSystem::saveModuleBuild(const std::string& buildPath, const std::string& content) {
    // 确保目录存在
    std::filesystem::create_directories(buildPath);
    
    std::ofstream file(buildPath + "/module.js");
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    file.close();
    return true;
}

std::vector<std::string> CMODSystem::parseDependencies(const std::string& source) const {
    std::vector<std::string> dependencies;
    
    // 简化的依赖解析
    size_t pos = 0;
    while ((pos = source.find("[Import]", pos)) != std::string::npos) {
        size_t start = pos + 8; // 跳过 "[Import]"
        size_t end = source.find("\n", start);
        if (end != std::string::npos) {
            std::string importLine = source.substr(start, end - start);
            // 提取模块名
            size_t spacePos = importLine.find(" ");
            if (spacePos != std::string::npos) {
                std::string moduleName = importLine.substr(0, spacePos);
                dependencies.push_back(moduleName);
            }
        }
        pos = end;
    }
    
    return dependencies;
}

std::vector<std::string> CMODSystem::parseExports(const std::string& source) const {
    std::vector<std::string> exports;
    
    // 简化的导出解析
    size_t pos = 0;
    while ((pos = source.find("[Export]", pos)) != std::string::npos) {
        size_t start = pos + 8; // 跳过 "[Export]"
        size_t end = source.find("\n", start);
        if (end != std::string::npos) {
            std::string exportLine = source.substr(start, end - start);
            // 提取导出名
            size_t spacePos = exportLine.find(" ");
            if (spacePos != std::string::npos) {
                std::string exportName = exportLine.substr(0, spacePos);
                exports.push_back(exportName);
            }
        }
        pos = end;
    }
    
    return exports;
}

std::vector<std::string> CMODSystem::parseImports(const std::string& source) const {
    return parseDependencies(source); // 导入和依赖使用相同的解析逻辑
}

bool CMODSystem::resolveDependencies(const CMODInfo& moduleInfo) {
    for (const auto& dependency : moduleInfo.dependencies) {
        if (!hasModule(dependency)) {
            if (debugMode) {
                std::cout << "Dependency not found: " << dependency << std::endl;
            }
            return false;
        }
    }
    return true;
}

bool CMODSystem::buildModule(const CMODInfo& moduleInfo) {
    std::string source;
    if (!loadModuleSource(moduleInfo.sourcePath, source)) {
        return false;
    }
    
    // 这里应该调用CHTL编译器来构建模块
    // 简化实现，直接保存源代码
    std::string buildContent = "// Built module: " + moduleInfo.name + "\n";
    buildContent += "// Version: " + moduleInfo.version + "\n";
    buildContent += source;
    
    return saveModuleBuild(moduleInfo.buildPath, buildContent);
}

bool CMODSystem::packageModule(const CMODInfo& moduleInfo) {
    // 简化的打包实现
    std::string packageContent = "// CMOD Package: " + moduleInfo.name + "\n";
    packageContent += "// Version: " + moduleInfo.version + "\n";
    packageContent += "// Description: " + moduleInfo.description + "\n";
    
    return saveModuleBuild(moduleInfo.buildPath, packageContent);
}

bool CMODSystem::registerModule(const CMODInfo& moduleInfo) {
    if (!validateModule(moduleInfo)) {
        return false;
    }
    
    std::string moduleId = generateModuleId(moduleInfo.name, moduleInfo.version);
    modules[moduleId] = moduleInfo;
    
    // 创建模块上下文
    auto context = std::make_shared<CHTLContext>(ContextType::NAMESPACE, moduleInfo.name);
    moduleContexts[moduleId] = context;
    
    if (debugMode) {
        std::cout << "Module registered: " << moduleId << std::endl;
    }
    
    return true;
}

bool CMODSystem::unregisterModule(const std::string& moduleName) {
    auto it = std::find_if(modules.begin(), modules.end(),
        [&moduleName](const auto& pair) {
            return pair.second.name == moduleName;
        });
    
    if (it != modules.end()) {
        modules.erase(it);
        moduleContexts.erase(it->first);
        return true;
    }
    
    return false;
}

bool CMODSystem::hasModule(const std::string& moduleName) const {
    return std::any_of(modules.begin(), modules.end(),
        [&moduleName](const auto& pair) {
            return pair.second.name == moduleName;
        });
}

CMODInfo CMODSystem::getModule(const std::string& moduleName) const {
    auto it = std::find_if(modules.begin(), modules.end(),
        [&moduleName](const auto& pair) {
            return pair.second.name == moduleName;
        });
    
    if (it != modules.end()) {
        return it->second;
    }
    
    return CMODInfo();
}

std::vector<CMODInfo> CMODSystem::getAllModules() const {
    std::vector<CMODInfo> result;
    for (const auto& pair : modules) {
        result.push_back(pair.second);
    }
    return result;
}

std::vector<CMODInfo> CMODSystem::getModulesByType(CMODType type) const {
    std::vector<CMODInfo> result;
    for (const auto& pair : modules) {
        if (pair.second.type == type) {
            result.push_back(pair.second);
        }
    }
    return result;
}

bool CMODSystem::loadModule(const std::string& moduleName) {
    std::string modulePath = getModulePath(moduleName);
    CMODInfo moduleInfo;
    
    if (!loadModuleInfo(modulePath, moduleInfo)) {
        return false;
    }
    
    moduleInfo.sourcePath = modulePath;
    moduleInfo.buildPath = getBuildPath(moduleName);
    
    return registerModule(moduleInfo);
}

bool CMODSystem::loadModule(const std::string& moduleName, const std::string& version) {
    std::string moduleId = generateModuleId(moduleName, version);
    auto it = modules.find(moduleId);
    
    if (it != modules.end()) {
        return true; // 模块已加载
    }
    
    return loadModule(moduleName);
}

bool CMODSystem::loadModuleFromPath(const std::string& modulePath) {
    CMODInfo moduleInfo;
    
    if (!loadModuleInfo(modulePath, moduleInfo)) {
        return false;
    }
    
    moduleInfo.sourcePath = modulePath;
    moduleInfo.buildPath = getBuildPath(moduleInfo.name);
    
    return registerModule(moduleInfo);
}

bool CMODSystem::unloadModule(const std::string& moduleName) {
    return unregisterModule(moduleName);
}

bool CMODSystem::buildModule(const std::string& moduleName) {
    CMODInfo moduleInfo = getModule(moduleName);
    if (moduleInfo.name.empty()) {
        return false;
    }
    
    return buildModule(moduleInfo);
}

bool CMODSystem::buildAllModules() {
    bool success = true;
    for (const auto& pair : modules) {
        if (!buildModule(pair.second)) {
            success = false;
        }
    }
    return success;
}

bool CMODSystem::rebuildModule(const std::string& moduleName) {
    return buildModule(moduleName);
}

bool CMODSystem::packageModule(const std::string& moduleName) {
    CMODInfo moduleInfo = getModule(moduleName);
    if (moduleInfo.name.empty()) {
        return false;
    }
    
    return packageModule(moduleInfo);
}

bool CMODSystem::packageAllModules() {
    bool success = true;
    for (const auto& pair : modules) {
        if (!packageModule(pair.second)) {
            success = false;
        }
    }
    return success;
}

bool CMODSystem::unpackModule(const std::string& packagePath) {
    // 简化的解包实现
    return true;
}

bool CMODSystem::addDependency(const std::string& moduleName, const CMODDependency& dependency) {
    if (!validateDependency(dependency)) {
        return false;
    }
    
    CMODInfo moduleInfo = getModule(moduleName);
    if (moduleInfo.name.empty()) {
        return false;
    }
    
    moduleInfo.dependencies.push_back(dependency.name);
    modules[generateModuleId(moduleInfo.name, moduleInfo.version)] = moduleInfo;
    
    return true;
}

bool CMODSystem::removeDependency(const std::string& moduleName, const std::string& dependencyName) {
    CMODInfo moduleInfo = getModule(moduleName);
    if (moduleInfo.name.empty()) {
        return false;
    }
    
    auto it = std::find(moduleInfo.dependencies.begin(), moduleInfo.dependencies.end(), dependencyName);
    if (it != moduleInfo.dependencies.end()) {
        moduleInfo.dependencies.erase(it);
        modules[generateModuleId(moduleInfo.name, moduleInfo.version)] = moduleInfo;
        return true;
    }
    
    return false;
}

std::vector<CMODDependency> CMODSystem::getDependencies(const std::string& moduleName) const {
    CMODInfo moduleInfo = getModule(moduleName);
    std::vector<CMODDependency> dependencies;
    
    for (const auto& depName : moduleInfo.dependencies) {
        CMODDependency dep;
        dep.name = depName;
        dependencies.push_back(dep);
    }
    
    return dependencies;
}

bool CMODSystem::resolveDependencies(const std::string& moduleName) {
    CMODInfo moduleInfo = getModule(moduleName);
    if (moduleInfo.name.empty()) {
        return false;
    }
    
    return resolveDependencies(moduleInfo);
}

bool CMODSystem::addExport(const std::string& moduleName, const CMODExport& exportItem) {
    if (!validateExport(exportItem)) {
        return false;
    }
    
    CMODInfo moduleInfo = getModule(moduleName);
    if (moduleInfo.name.empty()) {
        return false;
    }
    
    moduleInfo.exports.push_back(exportItem.name);
    modules[generateModuleId(moduleInfo.name, moduleInfo.version)] = moduleInfo;
    
    return true;
}

bool CMODSystem::removeExport(const std::string& moduleName, const std::string& exportName) {
    CMODInfo moduleInfo = getModule(moduleName);
    if (moduleInfo.name.empty()) {
        return false;
    }
    
    auto it = std::find(moduleInfo.exports.begin(), moduleInfo.exports.end(), exportName);
    if (it != moduleInfo.exports.end()) {
        moduleInfo.exports.erase(it);
        modules[generateModuleId(moduleInfo.name, moduleInfo.version)] = moduleInfo;
        return true;
    }
    
    return false;
}

std::vector<CMODExport> CMODSystem::getExports(const std::string& moduleName) const {
    CMODInfo moduleInfo = getModule(moduleName);
    std::vector<CMODExport> exports;
    
    for (const auto& exportName : moduleInfo.exports) {
        CMODExport exportItem;
        exportItem.name = exportName;
        exports.push_back(exportItem);
    }
    
    return exports;
}

CMODExport CMODSystem::getExport(const std::string& moduleName, const std::string& exportName) const {
    std::vector<CMODExport> exports = getExports(moduleName);
    
    for (const auto& exportItem : exports) {
        if (exportItem.name == exportName) {
            return exportItem;
        }
    }
    
    return CMODExport();
}

bool CMODSystem::addImport(const std::string& moduleName, const CMODImport& importItem) {
    if (!validateImport(importItem)) {
        return false;
    }
    
    CMODInfo moduleInfo = getModule(moduleName);
    if (moduleInfo.name.empty()) {
        return false;
    }
    
    moduleInfo.imports.push_back(importItem.name);
    modules[generateModuleId(moduleInfo.name, moduleInfo.version)] = moduleInfo;
    
    return true;
}

bool CMODSystem::removeImport(const std::string& moduleName, const std::string& importName) {
    CMODInfo moduleInfo = getModule(moduleName);
    if (moduleInfo.name.empty()) {
        return false;
    }
    
    auto it = std::find(moduleInfo.imports.begin(), moduleInfo.imports.end(), importName);
    if (it != moduleInfo.imports.end()) {
        moduleInfo.imports.erase(it);
        modules[generateModuleId(moduleInfo.name, moduleInfo.version)] = moduleInfo;
        return true;
    }
    
    return false;
}

std::vector<CMODImport> CMODSystem::getImports(const std::string& moduleName) const {
    CMODInfo moduleInfo = getModule(moduleName);
    std::vector<CMODImport> imports;
    
    for (const auto& importName : moduleInfo.imports) {
        CMODImport importItem;
        importItem.name = importName;
        imports.push_back(importItem);
    }
    
    return imports;
}

bool CMODSystem::resolveImports(const std::string& moduleName) {
    return resolveDependencies(moduleName);
}

std::shared_ptr<CHTLContext> CMODSystem::getModuleContext(const std::string& moduleName) const {
    auto it = std::find_if(moduleContexts.begin(), moduleContexts.end(),
        [&moduleName](const auto& pair) {
            return pair.second->getName() == moduleName;
        });
    
    if (it != moduleContexts.end()) {
        return it->second;
    }
    
    return nullptr;
}

bool CMODSystem::setModuleContext(const std::string& moduleName, std::shared_ptr<CHTLContext> context) {
    auto it = std::find_if(modules.begin(), modules.end(),
        [&moduleName](const auto& pair) {
            return pair.second.name == moduleName;
        });
    
    if (it != modules.end()) {
        moduleContexts[it->first] = context;
        return true;
    }
    
    return false;
}

void CMODSystem::setModulePath(const std::string& path) {
    modulePath = path;
}

void CMODSystem::setBuildPath(const std::string& path) {
    buildPath = path;
}

std::string CMODSystem::getModulePath() const {
    return modulePath;
}

std::string CMODSystem::getBuildPath() const {
    return buildPath;
}

void CMODSystem::setDebugMode(bool debug) {
    debugMode = debug;
}

bool CMODSystem::isDebugMode() const {
    return debugMode;
}

void CMODSystem::clear() {
    modules.clear();
    moduleContexts.clear();
}

void CMODSystem::reset() {
    clear();
    modulePath = "./modules";
    buildPath = "./build";
    debugMode = false;
}

void CMODSystem::printModules() const {
    std::cout << "=== CMOD Modules ===" << std::endl;
    for (const auto& pair : modules) {
        std::cout << "Module: " << pair.second.name 
                  << " v" << pair.second.version 
                  << " (" << static_cast<int>(pair.second.type) << ")" << std::endl;
    }
    std::cout << "===================" << std::endl;
}

void CMODSystem::printModuleInfo(const std::string& moduleName) const {
    CMODInfo moduleInfo = getModule(moduleName);
    if (moduleInfo.name.empty()) {
        std::cout << "Module not found: " << moduleName << std::endl;
        return;
    }
    
    std::cout << "=== Module Info: " << moduleName << " ===" << std::endl;
    std::cout << "Name: " << moduleInfo.name << std::endl;
    std::cout << "Version: " << moduleInfo.version << std::endl;
    std::cout << "Description: " << moduleInfo.description << std::endl;
    std::cout << "Author: " << moduleInfo.author << std::endl;
    std::cout << "License: " << moduleInfo.license << std::endl;
    std::cout << "Type: " << static_cast<int>(moduleInfo.type) << std::endl;
    std::cout << "Dependencies: " << moduleInfo.dependencies.size() << std::endl;
    std::cout << "Exports: " << moduleInfo.exports.size() << std::endl;
    std::cout << "Imports: " << moduleInfo.imports.size() << std::endl;
    std::cout << "=========================" << std::endl;
}

void CMODSystem::printDependencies(const std::string& moduleName) const {
    std::vector<CMODDependency> dependencies = getDependencies(moduleName);
    std::cout << "=== Dependencies: " << moduleName << " ===" << std::endl;
    for (const auto& dep : dependencies) {
        std::cout << "- " << dep.name << " (" << dep.version << ")" << std::endl;
    }
    std::cout << "=============================" << std::endl;
}

void CMODSystem::printExports(const std::string& moduleName) const {
    std::vector<CMODExport> exports = getExports(moduleName);
    std::cout << "=== Exports: " << moduleName << " ===" << std::endl;
    for (const auto& exp : exports) {
        std::cout << "- " << exp.name << " (" << exp.type << ")" << std::endl;
    }
    std::cout << "=======================" << std::endl;
}

void CMODSystem::printImports(const std::string& moduleName) const {
    std::vector<CMODImport> imports = getImports(moduleName);
    std::cout << "=== Imports: " << moduleName << " ===" << std::endl;
    for (const auto& imp : imports) {
        std::cout << "- " << imp.name << " from " << imp.source << std::endl;
    }
    std::cout << "=======================" << std::endl;
}

void CMODSystem::printStatistics() const {
    std::cout << "=== CMOD System Statistics ===" << std::endl;
    std::cout << "Total modules: " << modules.size() << std::endl;
    std::cout << "Module path: " << modulePath << std::endl;
    std::cout << "Build path: " << buildPath << std::endl;
    std::cout << "Debug mode: " << (debugMode ? "enabled" : "disabled") << std::endl;
    std::cout << "=============================" << std::endl;
}

} // namespace CHTL