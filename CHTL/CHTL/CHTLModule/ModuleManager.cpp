#include "ModuleManager.hpp"
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>
#include <filesystem>

namespace CHTL {

ModuleManager& ModuleManager::getInstance() {
    static ModuleManager instance;
    return instance;
}

bool ModuleManager::loadModule(const std::string& name, const std::string& path) {
    if (isModuleLoaded(name)) {
        return true;
    }
    
    std::string modulePath = path;
    if (modulePath.empty()) {
        modulePath = searchModuleFile(name, ModuleType::CMOD);
    }
    
    if (modulePath.empty()) {
        handleModuleError(name, "Module not found: " + name);
        return false;
    }
    
    ModuleType type = getModuleType(modulePath);
    
    bool success = false;
    switch (type) {
        case ModuleType::CMOD:
            success = loadCMODModule(name, modulePath);
            break;
        case ModuleType::CJMOD:
            success = loadCJMODModule(name, modulePath);
            break;
        case ModuleType::MIXED:
            success = loadMixedModule(name, modulePath);
            break;
        case ModuleType::CUSTOM:
            success = loadCustomModule(name, modulePath);
            break;
        default:
            handleModuleError(name, "Unknown module type");
            return false;
    }
    
    if (success) {
        auto moduleInfo = getModuleInfo(name);
        if (moduleInfo) {
            createModuleCache(name, *moduleInfo);
        }
    }
    
    return success;
}

void ModuleManager::unloadModule(const std::string& name) {
    auto moduleInfo = getModuleInfo(name);
    if (moduleInfo) {
        cleanupModuleResources(name);
        modules_.erase(name);
        deleteModuleCache(name);
    }
}

bool ModuleManager::reloadModule(const std::string& name) {
    auto moduleInfo = getModuleInfo(name);
    if (!moduleInfo) {
        return false;
    }
    
    std::string path = moduleInfo->path;
    unloadModule(name);
    return loadModule(name, path);
}

std::shared_ptr<ModuleManager::ModuleInfo> ModuleManager::getModuleInfo(const std::string& name) const {
    auto it = modules_.find(name);
    return (it != modules_.end()) ? it->second : nullptr;
}

bool ModuleManager::isModuleLoaded(const std::string& name) const {
    auto moduleInfo = getModuleInfo(name);
    return moduleInfo && moduleInfo->status == ModuleStatus::LOADED;
}

ModuleManager::ModuleStatus ModuleManager::getModuleStatus(const std::string& name) const {
    auto moduleInfo = getModuleInfo(name);
    return moduleInfo ? moduleInfo->status : ModuleStatus::UNLOADED;
}

std::vector<std::string> ModuleManager::getLoadedModules() const {
    std::vector<std::string> loadedModules;
    for (const auto& [name, moduleInfo] : modules_) {
        if (moduleInfo->status == ModuleStatus::LOADED) {
            loadedModules.push_back(name);
        }
    }
    return loadedModules;
}

std::vector<std::string> ModuleManager::getModuleDependencies(const std::string& name) const {
    auto moduleInfo = getModuleInfo(name);
    return moduleInfo ? moduleInfo->dependencies : std::vector<std::string>();
}

bool ModuleManager::checkModuleDependencies(const std::string& name) const {
    auto dependencies = getModuleDependencies(name);
    for (const auto& dependency : dependencies) {
        if (!isModuleLoaded(dependency)) {
            return false;
        }
    }
    return true;
}

std::vector<std::string> ModuleManager::resolveModuleDependencies(const std::string& name) const {
    std::vector<std::string> resolved;
    std::set<std::string> visited;
    
    std::function<void(const std::string&)> resolve = [&](const std::string& moduleName) {
        if (visited.count(moduleName)) {
            return;
        }
        visited.insert(moduleName);
        
        auto dependencies = getModuleDependencies(moduleName);
        for (const auto& dependency : dependencies) {
            resolve(dependency);
        }
        resolved.push_back(moduleName);
    };
    
    resolve(name);
    return resolved;
}

std::map<std::string, std::string> ModuleManager::getModuleExports(const std::string& name) const {
    auto moduleInfo = getModuleInfo(name);
    return moduleInfo ? moduleInfo->exports : std::map<std::string, std::string>();
}

std::map<std::string, std::string> ModuleManager::getModuleImports(const std::string& name) const {
    auto moduleInfo = getModuleInfo(name);
    return moduleInfo ? moduleInfo->imports : std::map<std::string, std::string>();
}

std::vector<std::string> ModuleManager::searchModules(const std::string& query) const {
    std::vector<std::string> results;
    std::regex queryRegex(query, std::regex_constants::icase);
    
    for (const auto& [name, moduleInfo] : modules_) {
        if (std::regex_search(name, queryRegex) ||
            std::regex_search(moduleInfo->description, queryRegex) ||
            std::regex_search(moduleInfo->author, queryRegex)) {
            results.push_back(name);
        }
    }
    
    return results;
}

std::vector<std::string> ModuleManager::getModuleList() const {
    std::vector<std::string> moduleList;
    for (const auto& [name, moduleInfo] : modules_) {
        moduleList.push_back(name);
    }
    return moduleList;
}

std::vector<std::string> ModuleManager::getModulesByType(ModuleType type) const {
    std::vector<std::string> modules;
    for (const auto& [name, moduleInfo] : modules_) {
        if (moduleInfo->type == type) {
            modules.push_back(name);
        }
    }
    return modules;
}

std::vector<std::string> ModuleManager::getModulesByCategory(const std::string& category) const {
    std::vector<std::string> modules;
    for (const auto& [name, moduleInfo] : modules_) {
        if (moduleInfo->category == category) {
            modules.push_back(name);
        }
    }
    return modules;
}

bool ModuleManager::validateModule(const std::string& name) const {
    auto moduleInfo = getModuleInfo(name);
    if (!moduleInfo) {
        return false;
    }
    
    return validateModuleInfo(*moduleInfo);
}

std::vector<std::string> ModuleManager::getModuleValidationErrors(const std::string& name) const {
    std::vector<std::string> errors;
    auto moduleInfo = getModuleInfo(name);
    
    if (!moduleInfo) {
        errors.push_back("Module not found: " + name);
        return errors;
    }
    
    // 验证基本信息
    if (moduleInfo->name.empty()) {
        errors.push_back("Module name is empty");
    }
    
    if (moduleInfo->version.empty()) {
        errors.push_back("Module version is empty");
    }
    
    if (moduleInfo->description.empty()) {
        errors.push_back("Module description is empty");
    }
    
    if (moduleInfo->author.empty()) {
        errors.push_back("Module author is empty");
    }
    
    if (moduleInfo->license.empty()) {
        errors.push_back("Module license is empty");
    }
    
    // 验证版本兼容性
    if (!moduleInfo->minCHTLVersion.empty() && !moduleInfo->maxCHTLVersion.empty()) {
        if (!checkVersionCompatibility("1.0.0", moduleInfo->minCHTLVersion, moduleInfo->maxCHTLVersion)) {
            errors.push_back("Module version compatibility check failed");
        }
    }
    
    // 验证依赖
    for (const auto& dependency : moduleInfo->dependencies) {
        if (!isModuleLoaded(dependency)) {
            errors.push_back("Missing dependency: " + dependency);
        }
    }
    
    return errors;
}

bool ModuleManager::packageModule(const std::string& name, const std::string& outputPath) const {
    auto moduleInfo = getModuleInfo(name);
    if (!moduleInfo) {
        return false;
    }
    
    // 创建模块包
    std::ofstream packageFile(outputPath, std::ios::binary);
    if (!packageFile.is_open()) {
        return false;
    }
    
    // 写入模块信息
    packageFile << "CHTL_MODULE_PACKAGE\n";
    packageFile << "NAME:" << moduleInfo->name << "\n";
    packageFile << "VERSION:" << moduleInfo->version << "\n";
    packageFile << "DESCRIPTION:" << moduleInfo->description << "\n";
    packageFile << "AUTHOR:" << moduleInfo->author << "\n";
    packageFile << "LICENSE:" << moduleInfo->license << "\n";
    packageFile << "TYPE:" << static_cast<int>(moduleInfo->type) << "\n";
    
    // 写入依赖
    packageFile << "DEPENDENCIES:";
    for (size_t i = 0; i < moduleInfo->dependencies.size(); ++i) {
        if (i > 0) packageFile << ",";
        packageFile << moduleInfo->dependencies[i];
    }
    packageFile << "\n";
    
    // 写入导出
    packageFile << "EXPORTS:";
    for (const auto& [key, value] : moduleInfo->exports) {
        packageFile << key << "=" << value << ";";
    }
    packageFile << "\n";
    
    packageFile.close();
    return true;
}

bool ModuleManager::unpackModule(const std::string& packagePath, const std::string& outputPath) const {
    std::ifstream packageFile(packagePath, std::ios::binary);
    if (!packageFile.is_open()) {
        return false;
    }
    
    std::string line;
    std::getline(packageFile, line);
    if (line != "CHTL_MODULE_PACKAGE") {
        return false;
    }
    
    // 创建输出目录
    std::filesystem::create_directories(outputPath);
    
    // 解析包内容
    while (std::getline(packageFile, line)) {
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);
            
            if (key == "NAME") {
                // 创建模块目录
                std::filesystem::create_directories(outputPath + "/" + value);
            }
        }
    }
    
    packageFile.close();
    return true;
}

bool ModuleManager::installModule(const std::string& packagePath) const {
    std::string tempPath = moduleCachePath_ + "/temp";
    if (!unpackModule(packagePath, tempPath)) {
        return false;
    }
    
    // 移动模块到正确位置
    std::filesystem::path packageFile(packagePath);
    std::string moduleName = packageFile.stem().string();
    std::string targetPath = moduleCachePath_ + "/" + moduleName;
    
    std::filesystem::rename(tempPath, targetPath);
    return true;
}

bool ModuleManager::uninstallModule(const std::string& name) const {
    std::string modulePath = moduleCachePath_ + "/" + name;
    if (std::filesystem::exists(modulePath)) {
        std::filesystem::remove_all(modulePath);
        return true;
    }
    return false;
}

bool ModuleManager::updateModule(const std::string& name) const {
    // 检查更新
    auto moduleInfo = getModuleInfo(name);
    if (!moduleInfo) {
        return false;
    }
    
    // 重新加载模块
    return reloadModule(name);
}

void ModuleManager::addModuleSearchPath(const std::string& path) {
    if (std::find(moduleSearchPaths_.begin(), moduleSearchPaths_.end(), path) == moduleSearchPaths_.end()) {
        moduleSearchPaths_.push_back(path);
    }
}

std::vector<std::string> ModuleManager::getModuleSearchPaths() const {
    return moduleSearchPaths_;
}

void ModuleManager::clearModuleSearchPaths() {
    moduleSearchPaths_.clear();
}

void ModuleManager::setModuleCachePath(const std::string& path) {
    moduleCachePath_ = path;
    std::filesystem::create_directories(path);
}

std::string ModuleManager::getModuleCachePath() const {
    return moduleCachePath_;
}

void ModuleManager::clearModuleCache() {
    if (std::filesystem::exists(moduleCachePath_)) {
        std::filesystem::remove_all(moduleCachePath_);
        std::filesystem::create_directories(moduleCachePath_);
    }
}

std::map<std::string, int> ModuleManager::getModuleStatistics() const {
    std::map<std::string, int> stats;
    
    stats["total_modules"] = modules_.size();
    stats["loaded_modules"] = 0;
    stats["cmod_modules"] = 0;
    stats["cjmod_modules"] = 0;
    stats["mixed_modules"] = 0;
    stats["custom_modules"] = 0;
    
    for (const auto& [name, moduleInfo] : modules_) {
        if (moduleInfo->status == ModuleStatus::LOADED) {
            stats["loaded_modules"]++;
        }
        
        switch (moduleInfo->type) {
            case ModuleType::CMOD:
                stats["cmod_modules"]++;
                break;
            case ModuleType::CJMOD:
                stats["cjmod_modules"]++;
                break;
            case ModuleType::MIXED:
                stats["mixed_modules"]++;
                break;
            case ModuleType::CUSTOM:
                stats["custom_modules"]++;
                break;
        }
    }
    
    return stats;
}

std::map<std::string, std::string> ModuleManager::exportModuleConfig() const {
    std::map<std::string, std::string> config;
    
    config["module_cache_path"] = moduleCachePath_;
    config["search_paths"] = "";
    for (size_t i = 0; i < moduleSearchPaths_.size(); ++i) {
        if (i > 0) config["search_paths"] += ";";
        config["search_paths"] += moduleSearchPaths_[i];
    }
    
    return config;
}

bool ModuleManager::importModuleConfig(const std::map<std::string, std::string>& config) {
    auto it = config.find("module_cache_path");
    if (it != config.end()) {
        setModuleCachePath(it->second);
    }
    
    it = config.find("search_paths");
    if (it != config.end()) {
        clearModuleSearchPaths();
        std::string paths = it->second;
        size_t pos = 0;
        while ((pos = paths.find(';')) != std::string::npos) {
            std::string path = paths.substr(0, pos);
            addModuleSearchPath(path);
            paths.erase(0, pos + 1);
        }
        if (!paths.empty()) {
            addModuleSearchPath(paths);
        }
    }
    
    return true;
}

void ModuleManager::clear() {
    modules_.clear();
    moduleSearchPaths_.clear();
    moduleCachePath_ = "";
}

void ModuleManager::removeModule(const std::string& name) {
    unloadModule(name);
}

bool ModuleManager::loadCMODModule(const std::string& name, const std::string& path) {
    // 加载CMOD模块的具体实现
    auto moduleInfo = std::make_shared<ModuleInfo>();
    moduleInfo->name = name;
    moduleInfo->type = ModuleType::CMOD;
    moduleInfo->status = ModuleStatus::LOADING;
    moduleInfo->path = path;
    
    // 解析模块信息
    std::string infoPath = path + "/info/" + name + ".chtl";
    auto parsedInfo = parseModuleInfo(infoPath);
    if (parsedInfo) {
        *moduleInfo = *parsedInfo;
    }
    
    moduleInfo->status = ModuleStatus::LOADED;
    modules_[name] = moduleInfo;
    
    return true;
}

bool ModuleManager::loadCJMODModule(const std::string& name, const std::string& path) {
    // 加载CJMOD模块的具体实现
    auto moduleInfo = std::make_shared<ModuleInfo>();
    moduleInfo->name = name;
    moduleInfo->type = ModuleType::CJMOD;
    moduleInfo->status = ModuleStatus::LOADING;
    moduleInfo->path = path;
    
    // 解析模块信息
    std::string infoPath = path + "/info/" + name + ".chtl";
    auto parsedInfo = parseModuleInfo(infoPath);
    if (parsedInfo) {
        *moduleInfo = *parsedInfo;
    }
    
    moduleInfo->status = ModuleStatus::LOADED;
    modules_[name] = moduleInfo;
    
    return true;
}

bool ModuleManager::loadMixedModule(const std::string& name, const std::string& path) {
    // 加载混合模块的具体实现
    auto moduleInfo = std::make_shared<ModuleInfo>();
    moduleInfo->name = name;
    moduleInfo->type = ModuleType::MIXED;
    moduleInfo->status = ModuleStatus::LOADING;
    moduleInfo->path = path;
    
    // 解析模块信息
    std::string infoPath = path + "/info/" + name + ".chtl";
    auto parsedInfo = parseModuleInfo(infoPath);
    if (parsedInfo) {
        *moduleInfo = *parsedInfo;
    }
    
    moduleInfo->status = ModuleStatus::LOADED;
    modules_[name] = moduleInfo;
    
    return true;
}

bool ModuleManager::loadCustomModule(const std::string& name, const std::string& path) {
    // 加载自定义模块的具体实现
    auto moduleInfo = std::make_shared<ModuleInfo>();
    moduleInfo->name = name;
    moduleInfo->type = ModuleType::CUSTOM;
    moduleInfo->status = ModuleStatus::LOADING;
    moduleInfo->path = path;
    
    // 解析模块信息
    std::string infoPath = path + "/info/" + name + ".chtl";
    auto parsedInfo = parseModuleInfo(infoPath);
    if (parsedInfo) {
        *moduleInfo = *parsedInfo;
    }
    
    moduleInfo->status = ModuleStatus::LOADED;
    modules_[name] = moduleInfo;
    
    return true;
}

std::shared_ptr<ModuleManager::ModuleInfo> ModuleManager::parseModuleInfo(const std::string& infoPath) const {
    auto moduleInfo = std::make_shared<ModuleInfo>();
    
    std::ifstream file(infoPath);
    if (!file.is_open()) {
        return nullptr;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        size_t equalPos = line.find('=');
        if (equalPos != std::string::npos) {
            std::string key = line.substr(0, equalPos);
            std::string value = line.substr(equalPos + 1);
            
            // 移除引号
            if (value.length() >= 2 && value[0] == '"' && value[value.length() - 1] == '"') {
                value = value.substr(1, value.length() - 2);
            }
            
            if (key == "name") {
                moduleInfo->name = value;
            } else if (key == "version") {
                moduleInfo->version = value;
            } else if (key == "description") {
                moduleInfo->description = value;
            } else if (key == "author") {
                moduleInfo->author = value;
            } else if (key == "license") {
                moduleInfo->license = value;
            } else if (key == "category") {
                moduleInfo->category = value;
            } else if (key == "minCHTLVersion") {
                moduleInfo->minCHTLVersion = value;
            } else if (key == "maxCHTLVersion") {
                moduleInfo->maxCHTLVersion = value;
            }
        }
    }
    
    file.close();
    return moduleInfo;
}

ModuleManager::ModuleStructure ModuleManager::parseModuleStructure(const std::string& modulePath) const {
    ModuleStructure structure;
    structure.srcPath = modulePath + "/src";
    structure.infoPath = modulePath + "/info";
    
    // 扫描源文件
    if (std::filesystem::exists(structure.srcPath)) {
        for (const auto& entry : std::filesystem::directory_iterator(structure.srcPath)) {
            if (entry.is_regular_file()) {
                structure.sourceFiles.push_back(entry.path().string());
            }
        }
    }
    
    // 扫描信息文件
    if (std::filesystem::exists(structure.infoPath)) {
        for (const auto& entry : std::filesystem::directory_iterator(structure.infoPath)) {
            if (entry.is_regular_file()) {
                structure.infoFiles.push_back(entry.path().string());
            }
        }
    }
    
    return structure;
}

bool ModuleManager::validateModuleStructure(const ModuleStructure& structure) const {
    // 验证源路径
    if (!std::filesystem::exists(structure.srcPath)) {
        return false;
    }
    
    // 验证信息路径
    if (!std::filesystem::exists(structure.infoPath)) {
        return false;
    }
    
    // 验证源文件
    if (structure.sourceFiles.empty()) {
        return false;
    }
    
    // 验证信息文件
    if (structure.infoFiles.empty()) {
        return false;
    }
    
    return true;
}

bool ModuleManager::validateModuleInfo(const ModuleInfo& info) const {
    // 验证基本信息
    if (info.name.empty() || info.version.empty() || info.description.empty()) {
        return false;
    }
    
    // 验证版本格式
    std::regex versionRegex(R"(\d+\.\d+\.\d+)");
    if (!std::regex_match(info.version, versionRegex)) {
        return false;
    }
    
    return true;
}

std::string ModuleManager::searchModuleFile(const std::string& name, ModuleType type) const {
    for (const auto& searchPath : moduleSearchPaths_) {
        std::string modulePath = searchPath + "/" + name;
        if (std::filesystem::exists(modulePath)) {
            return modulePath;
        }
    }
    return "";
}

ModuleManager::ModuleType ModuleManager::getModuleType(const std::string& path) const {
    if (std::filesystem::exists(path + "/src") && std::filesystem::exists(path + "/info")) {
        return ModuleType::CMOD;
    } else if (std::filesystem::exists(path + "/cjmod")) {
        return ModuleType::CJMOD;
    } else if (std::filesystem::exists(path + "/mixed")) {
        return ModuleType::MIXED;
    } else {
        return ModuleType::CUSTOM;
    }
}

bool ModuleManager::createModuleCache(const std::string& name, const ModuleInfo& info) const {
    std::string cachePath = moduleCachePath_ + "/" + name;
    std::filesystem::create_directories(cachePath);
    
    std::ofstream cacheFile(cachePath + "/module.info");
    if (!cacheFile.is_open()) {
        return false;
    }
    
    cacheFile << "name=" << info.name << "\n";
    cacheFile << "version=" << info.version << "\n";
    cacheFile << "description=" << info.description << "\n";
    cacheFile << "author=" << info.author << "\n";
    cacheFile << "license=" << info.license << "\n";
    cacheFile << "type=" << static_cast<int>(info.type) << "\n";
    
    cacheFile.close();
    return true;
}

std::shared_ptr<ModuleManager::ModuleInfo> ModuleManager::loadModuleCache(const std::string& name) const {
    std::string cachePath = moduleCachePath_ + "/" + name + "/module.info";
    if (!std::filesystem::exists(cachePath)) {
        return nullptr;
    }
    
    return parseModuleInfo(cachePath);
}

bool ModuleManager::updateModuleCache(const std::string& name, const ModuleInfo& info) const {
    return createModuleCache(name, info);
}

bool ModuleManager::deleteModuleCache(const std::string& name) const {
    std::string cachePath = moduleCachePath_ + "/" + name;
    if (std::filesystem::exists(cachePath)) {
        std::filesystem::remove_all(cachePath);
        return true;
    }
    return false;
}

bool ModuleManager::checkVersionCompatibility(const std::string& moduleVersion, 
                                            const std::string& minVersion, 
                                            const std::string& maxVersion) const {
    auto moduleVer = parseVersion(moduleVersion);
    auto minVer = parseVersion(minVersion);
    auto maxVer = parseVersion(maxVersion);
    
    return compareVersions(moduleVer, minVer) >= 0 && compareVersions(moduleVer, maxVer) <= 0;
}

std::vector<int> ModuleManager::parseVersion(const std::string& version) const {
    std::vector<int> versionParts;
    std::stringstream ss(version);
    std::string part;
    
    while (std::getline(ss, part, '.')) {
        try {
            versionParts.push_back(std::stoi(part));
        } catch (const std::exception&) {
            versionParts.push_back(0);
        }
    }
    
    return versionParts;
}

int ModuleManager::compareVersions(const std::vector<int>& version1, const std::vector<int>& version2) const {
    size_t maxSize = std::max(version1.size(), version2.size());
    
    for (size_t i = 0; i < maxSize; ++i) {
        int v1 = (i < version1.size()) ? version1[i] : 0;
        int v2 = (i < version2.size()) ? version2[i] : 0;
        
        if (v1 < v2) return -1;
        if (v1 > v2) return 1;
    }
    
    return 0;
}

void ModuleManager::handleModuleError(const std::string& name, const std::string& error) {
    // 处理模块错误
    std::cerr << "Module Error [" << name << "]: " << error << std::endl;
}

void ModuleManager::handleModuleWarning(const std::string& name, const std::string& warning) {
    // 处理模块警告
    std::cout << "Module Warning [" << name << "]: " << warning << std::endl;
}

void ModuleManager::cleanupModuleResources(const std::string& name) {
    // 清理模块资源
    // 这里可以添加清理逻辑
}

void ModuleManager::initializeModuleSystem() {
    // 初始化模块系统
    if (moduleCachePath_.empty()) {
        moduleCachePath_ = "./chtl/modules";
    }
    
    std::filesystem::create_directories(moduleCachePath_);
    
    // 添加默认搜索路径
    addModuleSearchPath("./chtl/modules");
    addModuleSearchPath("./modules");
    addModuleSearchPath("./chtl/module");
    
    // 扫描模块目录
    scanModuleDirectories();
    
    // 加载模块索引
    loadModuleIndex();
}

void ModuleManager::scanModuleDirectories() {
    for (const auto& searchPath : moduleSearchPaths_) {
        if (std::filesystem::exists(searchPath)) {
            for (const auto& entry : std::filesystem::directory_iterator(searchPath)) {
                if (entry.is_directory()) {
                    std::string moduleName = entry.path().filename().string();
                    std::string modulePath = entry.path().string();
                    
                    ModuleType type = getModuleType(modulePath);
                    if (type != ModuleType::CUSTOM) {
                        loadModule(moduleName, modulePath);
                    }
                }
            }
        }
    }
}

void ModuleManager::updateModuleIndex() {
    // 更新模块索引
    // 这里可以添加索引更新逻辑
}

bool ModuleManager::saveModuleIndex() const {
    std::string indexPath = moduleCachePath_ + "/index.json";
    std::ofstream indexFile(indexPath);
    if (!indexFile.is_open()) {
        return false;
    }
    
    // 保存模块索引
    indexFile << "{\n";
    indexFile << "  \"modules\": [\n";
    
    bool first = true;
    for (const auto& [name, moduleInfo] : modules_) {
        if (!first) indexFile << ",\n";
        indexFile << "    {\n";
        indexFile << "      \"name\": \"" << moduleInfo->name << "\",\n";
        indexFile << "      \"version\": \"" << moduleInfo->version << "\",\n";
        indexFile << "      \"type\": " << static_cast<int>(moduleInfo->type) << ",\n";
        indexFile << "      \"status\": " << static_cast<int>(moduleInfo->status) << "\n";
        indexFile << "    }";
        first = false;
    }
    
    indexFile << "\n  ]\n";
    indexFile << "}\n";
    
    indexFile.close();
    return true;
}

bool ModuleManager::loadModuleIndex() {
    std::string indexPath = moduleCachePath_ + "/index.json";
    if (!std::filesystem::exists(indexPath)) {
        return false;
    }
    
    // 加载模块索引
    // 这里可以添加索引加载逻辑
    
    return true;
}

} // namespace CHTL