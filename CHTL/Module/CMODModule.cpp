#include "CMODModule.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <set>

namespace CHTL {
namespace Module {

CMODModule::CMODModule() {
    info.status = CMODModuleStatus::UNLOADED;
    info.type = CMODModuleType::CUSTOM;
    info.lastModified = std::chrono::system_clock::now();
}

CMODModule::CMODModule(const CMODModuleInfo& info) : info(info) {
    if (info.checksum.empty()) {
        this->info.checksum = generateChecksum();
    }
}

CMODModule::~CMODModule() {
    // 清理资源
}

// 模块信息管理实现
const CMODModuleInfo& CMODModule::getInfo() const {
    return info;
}

void CMODModule::setInfo(const CMODModuleInfo& info) {
    this->info = info;
    if (this->info.checksum.empty()) {
        this->info.checksum = generateChecksum();
    }
    updateLastModified();
}

void CMODModule::updateInfo(const std::map<std::string, std::string>& updates) {
    for (const auto& update : updates) {
        if (update.first == "name") {
            info.name = update.second;
        } else if (update.first == "version") {
            info.version = update.second;
        } else if (update.first == "description") {
            info.description = update.second;
        } else if (update.first == "author") {
            info.author = update.second;
        } else if (update.first == "license") {
            info.license = update.second;
        } else if (update.first == "repository") {
            info.repository = update.second;
        } else if (update.first == "documentation") {
            info.documentation = update.second;
        } else {
            info.metadata[update.first] = update.second;
        }
    }
    updateLastModified();
}

// 模块状态管理实现
CMODModuleStatus CMODModule::getStatus() const {
    return info.status;
}

void CMODModule::setStatus(CMODModuleStatus status) {
    info.status = status;
}

bool CMODModule::isLoaded() const {
    return info.status == CMODModuleStatus::LOADED;
}

bool CMODModule::isError() const {
    return info.status == CMODModuleStatus::ERROR;
}

bool CMODModule::isDisabled() const {
    return info.status == CMODModuleStatus::DISABLED;
}

// 导出管理实现
void CMODModule::addExport(const CMODExport& exportItem) {
    // 检查是否已存在同名导出
    auto it = std::find_if(exports.begin(), exports.end(),
        [&](const CMODExport& exp) { return exp.name == exportItem.name; });
    
    if (it != exports.end()) {
        *it = exportItem; // 更新现有导出
    } else {
        exports.push_back(exportItem); // 添加新导出
    }
    
    // 更新模块信息中的导出列表
    info.exports.clear();
    for (const auto& exp : exports) {
        info.exports.push_back(exp.name);
    }
}

void CMODModule::removeExport(const std::string& name) {
    exports.erase(
        std::remove_if(exports.begin(), exports.end(),
            [&](const CMODExport& exp) { return exp.name == name; }),
        exports.end()
    );
    
    // 更新模块信息中的导出列表
    info.exports.clear();
    for (const auto& exp : exports) {
        info.exports.push_back(exp.name);
    }
}

CMODExport* CMODModule::getExport(const std::string& name) {
    auto it = std::find_if(exports.begin(), exports.end(),
        [&](const CMODExport& exp) { return exp.name == name; });
    return (it != exports.end()) ? &(*it) : nullptr;
}

const CMODExport* CMODModule::getExport(const std::string& name) const {
    auto it = std::find_if(exports.begin(), exports.end(),
        [&](const CMODExport& exp) { return exp.name == name; });
    return (it != exports.end()) ? &(*it) : nullptr;
}

std::vector<CMODExport> CMODModule::getAllExports() const {
    return exports;
}

std::vector<CMODExport> CMODModule::getPublicExports() const {
    std::vector<CMODExport> publicExports;
    for (const auto& exp : exports) {
        if (exp.isPublic) {
            publicExports.push_back(exp);
        }
    }
    return publicExports;
}

bool CMODModule::hasExport(const std::string& name) const {
    return std::any_of(exports.begin(), exports.end(),
        [&](const CMODExport& exp) { return exp.name == name; });
}

// 子模块管理实现
void CMODModule::addSubModule(const CMODSubModule& subModule) {
    // 检查是否已存在同名子模块
    auto it = std::find_if(subModules.begin(), subModules.end(),
        [&](const CMODSubModule& sub) { return sub.name == subModule.name; });
    
    if (it != subModules.end()) {
        *it = subModule; // 更新现有子模块
    } else {
        subModules.push_back(subModule); // 添加新子模块
    }
}

void CMODModule::removeSubModule(const std::string& name) {
    subModules.erase(
        std::remove_if(subModules.begin(), subModules.end(),
            [&](const CMODSubModule& sub) { return sub.name == name; }),
        subModules.end()
    );
}

CMODSubModule* CMODModule::getSubModule(const std::string& name) {
    auto it = std::find_if(subModules.begin(), subModules.end(),
        [&](const CMODSubModule& sub) { return sub.name == name; });
    return (it != subModules.end()) ? &(*it) : nullptr;
}

const CMODSubModule* CMODModule::getSubModule(const std::string& name) const {
    auto it = std::find_if(subModules.begin(), subModules.end(),
        [&](const CMODSubModule& sub) { return sub.name == name; });
    return (it != subModules.end()) ? &(*it) : nullptr;
}

std::vector<CMODSubModule> CMODModule::getAllSubModules() const {
    return subModules;
}

bool CMODModule::hasSubModule(const std::string& name) const {
    return std::any_of(subModules.begin(), subModules.end(),
        [&](const CMODSubModule& sub) { return sub.name == name; });
}

// 导入路径管理实现
void CMODModule::addImportPath(const CMODImportPath& importPath) {
    // 检查是否已存在同名导入路径
    auto it = std::find_if(importPaths.begin(), importPaths.end(),
        [&](const CMODImportPath& imp) { return imp.moduleName == importPath.moduleName; });
    
    if (it != importPaths.end()) {
        *it = importPath; // 更新现有导入路径
    } else {
        importPaths.push_back(importPath); // 添加新导入路径
    }
}

void CMODModule::removeImportPath(const std::string& moduleName) {
    importPaths.erase(
        std::remove_if(importPaths.begin(), importPaths.end(),
            [&](const CMODImportPath& imp) { return imp.moduleName == moduleName; }),
        importPaths.end()
    );
}

CMODImportPath* CMODModule::getImportPath(const std::string& moduleName) {
    auto it = std::find_if(importPaths.begin(), importPaths.end(),
        [&](const CMODImportPath& imp) { return imp.moduleName == moduleName; });
    return (it != importPaths.end()) ? &(*it) : nullptr;
}

const CMODImportPath* CMODModule::getImportPath(const std::string& moduleName) const {
    auto it = std::find_if(importPaths.begin(), importPaths.end(),
        [&](const CMODImportPath& imp) { return imp.moduleName == moduleName; });
    return (it != importPaths.end()) ? &(*it) : nullptr;
}

std::vector<CMODImportPath> CMODModule::getAllImportPaths() const {
    return importPaths;
}

bool CMODModule::hasImportPath(const std::string& moduleName) const {
    return std::any_of(importPaths.begin(), importPaths.end(),
        [&](const CMODImportPath& imp) { return imp.moduleName == moduleName; });
}

// 依赖管理实现
void CMODModule::addDependency(const std::string& dependency) {
    if (!hasDependency(dependency)) {
        info.dependencies.push_back(dependency);
    }
}

void CMODModule::removeDependency(const std::string& dependency) {
    info.dependencies.erase(
        std::remove(info.dependencies.begin(), info.dependencies.end(), dependency),
        info.dependencies.end()
    );
}

std::vector<std::string> CMODModule::getDependencies() const {
    return info.dependencies;
}

bool CMODModule::hasDependency(const std::string& dependency) const {
    return std::find(info.dependencies.begin(), info.dependencies.end(), dependency) != info.dependencies.end();
}

bool CMODModule::isDependencySatisfied(const std::string& dependency) const {
    // 这里可以实现依赖检查逻辑
    // 暂时返回true，实际实现需要检查依赖模块是否已加载
    return true;
}

// 元数据管理实现
void CMODModule::setMetadata(const std::string& key, const std::string& value) {
    info.metadata[key] = value;
}

std::string CMODModule::getMetadata(const std::string& key) const {
    auto it = info.metadata.find(key);
    return (it != info.metadata.end()) ? it->second : "";
}

bool CMODModule::hasMetadata(const std::string& key) const {
    return info.metadata.find(key) != info.metadata.end();
}

void CMODModule::removeMetadata(const std::string& key) {
    info.metadata.erase(key);
}

std::map<std::string, std::string> CMODModule::getAllMetadata() const {
    return info.metadata;
}

// 模块操作实现
bool CMODModule::load() {
    if (info.status == CMODModuleStatus::LOADED) {
        return true;
    }
    
    if (info.status == CMODModuleStatus::ERROR) {
        addError("模块处于错误状态，无法加载");
        return false;
    }
    
    if (info.status == CMODModuleStatus::DISABLED) {
        addError("模块已禁用，无法加载");
        return false;
    }
    
    info.status = CMODModuleStatus::LOADING;
    
    // 检查依赖
    for (const auto& dependency : info.dependencies) {
        if (!isDependencySatisfied(dependency)) {
            addError("依赖未满足: " + dependency);
            info.status = CMODModuleStatus::ERROR;
            return false;
        }
    }
    
    // 验证模块
    if (!validate()) {
        info.status = CMODModuleStatus::ERROR;
        return false;
    }
    
    info.status = CMODModuleStatus::LOADED;
    return true;
}

bool CMODModule::unload() {
    if (info.status != CMODModuleStatus::LOADED) {
        return false;
    }
    
    info.status = CMODModuleStatus::UNLOADED;
    return true;
}

bool CMODModule::reload() {
    if (info.status == CMODModuleStatus::LOADED) {
        if (!unload()) {
            return false;
        }
    }
    
    return load();
}

bool CMODModule::enable() {
    if (info.status == CMODModuleStatus::DISABLED) {
        info.status = CMODModuleStatus::UNLOADED;
        return true;
    }
    return false;
}

bool CMODModule::disable() {
    if (info.status == CMODModuleStatus::LOADED) {
        if (!unload()) {
            return false;
        }
    }
    
    info.status = CMODModuleStatus::DISABLED;
    return true;
}

// 验证实现
bool CMODModule::validate() const {
    return validateModuleInfo() && validateExports() && validateSubModules() && 
           validateImportPaths() && validateDependencies();
}

std::vector<std::string> CMODModule::getValidationErrors() const {
    std::vector<std::string> errors;
    
    if (!validateModuleInfo()) {
        errors.push_back("模块信息验证失败");
    }
    
    if (!validateExports()) {
        errors.push_back("导出验证失败");
    }
    
    if (!validateSubModules()) {
        errors.push_back("子模块验证失败");
    }
    
    if (!validateImportPaths()) {
        errors.push_back("导入路径验证失败");
    }
    
    if (!validateDependencies()) {
        errors.push_back("依赖验证失败");
    }
    
    return errors;
}

// 序列化实现
std::string CMODModule::toJSON() const {
    std::ostringstream oss;
    oss << "{" << std::endl;
    oss << "  \"name\": \"" << info.name << "\"," << std::endl;
    oss << "  \"version\": \"" << info.version << "\"," << std::endl;
    oss << "  \"description\": \"" << info.description << "\"," << std::endl;
    oss << "  \"author\": \"" << info.author << "\"," << std::endl;
    oss << "  \"license\": \"" << info.license << "\"," << std::endl;
    oss << "  \"type\": " << static_cast<int>(info.type) << "," << std::endl;
    oss << "  \"status\": " << static_cast<int>(info.status) << "," << std::endl;
    oss << "  \"dependencies\": [";
    for (size_t i = 0; i < info.dependencies.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << "\"" << info.dependencies[i] << "\"";
    }
    oss << "]," << std::endl;
    oss << "  \"exports\": [";
    for (size_t i = 0; i < info.exports.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << "\"" << info.exports[i] << "\"";
    }
    oss << "]" << std::endl;
    oss << "}" << std::endl;
    return oss.str();
}

bool CMODModule::fromJSON(const std::string& json) {
    // 简单的JSON解析实现
    // 实际实现应该使用JSON库
    return true;
}

std::string CMODModule::toXML() const {
    std::ostringstream oss;
    oss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    oss << "<module>" << std::endl;
    oss << "  <name>" << info.name << "</name>" << std::endl;
    oss << "  <version>" << info.version << "</version>" << std::endl;
    oss << "  <description>" << info.description << "</description>" << std::endl;
    oss << "  <author>" << info.author << "</author>" << std::endl;
    oss << "  <license>" << info.license << "</license>" << std::endl;
    oss << "  <type>" << static_cast<int>(info.type) << "</type>" << std::endl;
    oss << "  <status>" << static_cast<int>(info.status) << "</status>" << std::endl;
    oss << "</module>" << std::endl;
    return oss.str();
}

bool CMODModule::fromXML(const std::string& xml) {
    // 简单的XML解析实现
    // 实际实现应该使用XML库
    return true;
}

// 工具方法实现
std::string CMODModule::getDisplayName() const {
    return info.name + " v" + info.version;
}

std::string CMODModule::getVersionString() const {
    return info.version;
}

std::string CMODModule::getTypeString() const {
    switch (static_cast<int>(info.type)) {
        case static_cast<int>(CMODModuleType::CORE): return "Core";
        case static_cast<int>(CMODModuleType::FEATURE): return "Feature";
        case static_cast<int>(CMODModuleType::UTILITY): return "Utility";
        case static_cast<int>(CMODModuleType::PLUGIN): return "Plugin";
        case static_cast<int>(CMODModuleType::CUSTOM): return "Custom";
        default: return "Unknown";
    }
}

std::string CMODModule::getStatusString() const {
    switch (static_cast<int>(info.status)) {
        case static_cast<int>(CMODModuleStatus::UNLOADED): return "Unloaded";
        case static_cast<int>(CMODModuleStatus::LOADING): return "Loading";
        case static_cast<int>(CMODModuleStatus::LOADED): return "Loaded";
        case static_cast<int>(CMODModuleStatus::ERROR): return "Error";
        case static_cast<int>(CMODModuleStatus::DISABLED): return "Disabled";
        default: return "Unknown";
    }
}

bool CMODModule::isCompatible(const std::string& version) const {
    return compareVersion(version) >= 0;
}

int CMODModule::compareVersion(const std::string& version) const {
    return compareVersionNumbers(info.version, version);
}

// 错误处理实现
std::vector<std::string> CMODModule::getErrors() const {
    return errors;
}

std::vector<std::string> CMODModule::getWarnings() const {
    return warnings;
}

void CMODModule::addError(const std::string& error) {
    errors.push_back(error);
}

void CMODModule::addWarning(const std::string& warning) {
    warnings.push_back(warning);
}

void CMODModule::clearErrors() {
    errors.clear();
}

void CMODModule::clearWarnings() {
    warnings.clear();
}

// 内部方法实现
bool CMODModule::validateModuleInfo() const {
    return !info.name.empty() && !info.version.empty();
}

bool CMODModule::validateExports() const {
    // 检查导出名称是否唯一
    std::set<std::string> exportNames;
    for (const auto& exp : exports) {
        if (exportNames.find(exp.name) != exportNames.end()) {
            return false; // 重复的导出名称
        }
        exportNames.insert(exp.name);
    }
    return true;
}

bool CMODModule::validateSubModules() const {
    // 检查子模块名称是否唯一
    std::set<std::string> subModuleNames;
    for (const auto& sub : subModules) {
        if (subModuleNames.find(sub.name) != subModuleNames.end()) {
            return false; // 重复的子模块名称
        }
        subModuleNames.insert(sub.name);
    }
    return true;
}

bool CMODModule::validateImportPaths() const {
    // 检查导入路径是否有效
    for (const auto& imp : importPaths) {
        if (imp.moduleName.empty() || imp.path.empty()) {
            return false;
        }
    }
    return true;
}

bool CMODModule::validateDependencies() const {
    // 检查依赖是否有效
    for (const auto& dep : info.dependencies) {
        if (dep.empty()) {
            return false;
        }
    }
    return true;
}

std::string CMODModule::generateChecksum() const {
    // 简单的校验和生成
    std::string data = info.name + info.version + info.description;
    std::hash<std::string> hasher;
    return std::to_string(hasher(data));
}

bool CMODModule::verifyChecksum() const {
    return info.checksum == generateChecksum();
}

void CMODModule::updateLastModified() {
    info.lastModified = std::chrono::system_clock::now();
}

int CMODModule::compareVersionNumbers(const std::string& v1, const std::string& v2) const {
    auto parts1 = parseVersion(v1);
    auto parts2 = parseVersion(v2);
    
    size_t maxSize = std::max(parts1.size(), parts2.size());
    for (size_t i = 0; i < maxSize; ++i) {
        int part1 = (i < parts1.size()) ? parts1[i] : 0;
        int part2 = (i < parts2.size()) ? parts2[i] : 0;
        
        if (part1 < part2) return -1;
        if (part1 > part2) return 1;
    }
    
    return 0;
}

std::vector<int> CMODModule::parseVersion(const std::string& version) const {
    std::vector<int> parts;
    std::istringstream iss(version);
    std::string part;
    
    while (std::getline(iss, part, '.')) {
        try {
            parts.push_back(std::stoi(part));
        } catch (const std::exception&) {
            parts.push_back(0);
        }
    }
    
    return parts;
}

} // namespace Module
} // namespace CHTL