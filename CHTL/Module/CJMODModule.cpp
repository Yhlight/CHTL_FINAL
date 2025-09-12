#include "CJMODModule.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <set>
#include <regex>
#include <filesystem>

namespace CHTL {
namespace Module {

CJMODModule::CJMODModule() {
    info.status = CJMODModuleStatus::UNLOADED;
    info.type = CJMODModuleType::CUSTOM;
    info.lastModified = std::chrono::system_clock::now();
}

CJMODModule::CJMODModule(const CJMODModuleInfo& info) : info(info) {
    if (info.checksum.empty()) {
        this->info.checksum = generateChecksum();
    }
}

CJMODModule::~CJMODModule() {
    // 清理资源
}

// 模块信息管理实现
const CJMODModuleInfo& CJMODModule::getInfo() const {
    return info;
}

void CJMODModule::setInfo(const CJMODModuleInfo& info) {
    this->info = info;
    if (this->info.checksum.empty()) {
        this->info.checksum = generateChecksum();
    }
    updateLastModified();
}

void CJMODModule::updateInfo(const std::map<std::string, std::string>& updates) {
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
CJMODModuleStatus CJMODModule::getStatus() const {
    return info.status;
}

void CJMODModule::setStatus(CJMODModuleStatus status) {
    info.status = status;
}

bool CJMODModule::isLoaded() const {
    return info.status == CJMODModuleStatus::LOADED;
}

bool CJMODModule::isError() const {
    return info.status == CJMODModuleStatus::ERROR;
}

bool CJMODModule::isDisabled() const {
    return info.status == CJMODModuleStatus::DISABLED;
}

// API管理实现
void CJMODModule::addAPIItem(const CJMODAPIItem& apiItem) {
    // 检查是否已存在同名API项
    auto it = std::find_if(apiItems.begin(), apiItems.end(),
        [&](const CJMODAPIItem& item) { return item.name == apiItem.name; });
    
    if (it != apiItems.end()) {
        *it = apiItem; // 更新现有API项
    } else {
        apiItems.push_back(apiItem); // 添加新API项
    }
    
    // 更新模块信息中的导出列表
    info.exports.clear();
    for (const auto& item : apiItems) {
        if (item.isPublic) {
            info.exports.push_back(item.name);
        }
    }
}

void CJMODModule::removeAPIItem(const std::string& name) {
    apiItems.erase(
        std::remove_if(apiItems.begin(), apiItems.end(),
            [&](const CJMODAPIItem& item) { return item.name == name; }),
        apiItems.end()
    );
    
    // 更新模块信息中的导出列表
    info.exports.clear();
    for (const auto& item : apiItems) {
        if (item.isPublic) {
            info.exports.push_back(item.name);
        }
    }
}

CJMODAPIItem* CJMODModule::getAPIItem(const std::string& name) {
    auto it = std::find_if(apiItems.begin(), apiItems.end(),
        [&](const CJMODAPIItem& item) { return item.name == name; });
    return (it != apiItems.end()) ? &(*it) : nullptr;
}

const CJMODAPIItem* CJMODModule::getAPIItem(const std::string& name) const {
    auto it = std::find_if(apiItems.begin(), apiItems.end(),
        [&](const CJMODAPIItem& item) { return item.name == name; });
    return (it != apiItems.end()) ? &(*it) : nullptr;
}

std::vector<CJMODAPIItem> CJMODModule::getAllAPIItems() const {
    return apiItems;
}

std::vector<CJMODAPIItem> CJMODModule::getPublicAPIItems() const {
    std::vector<CJMODAPIItem> publicItems;
    for (const auto& item : apiItems) {
        if (item.isPublic) {
            publicItems.push_back(item);
        }
    }
    return publicItems;
}

std::vector<CJMODAPIItem> CJMODModule::getAPIItemsByType(CJMODAPIType type) const {
    std::vector<CJMODAPIItem> items;
    for (const auto& item : apiItems) {
        if (item.type == type) {
            items.push_back(item);
        }
    }
    return items;
}

bool CJMODModule::hasAPIItem(const std::string& name) const {
    return std::any_of(apiItems.begin(), apiItems.end(),
        [&](const CJMODAPIItem& item) { return item.name == name; });
}

// 子模块管理实现
void CJMODModule::addSubModule(const CJMODSubModule& subModule) {
    // 检查是否已存在同名子模块
    auto it = std::find_if(subModules.begin(), subModules.end(),
        [&](const CJMODSubModule& sub) { return sub.name == subModule.name; });
    
    if (it != subModules.end()) {
        *it = subModule; // 更新现有子模块
    } else {
        subModules.push_back(subModule); // 添加新子模块
    }
}

void CJMODModule::removeSubModule(const std::string& name) {
    subModules.erase(
        std::remove_if(subModules.begin(), subModules.end(),
            [&](const CJMODSubModule& sub) { return sub.name == name; }),
        subModules.end()
    );
}

CJMODSubModule* CJMODModule::getSubModule(const std::string& name) {
    auto it = std::find_if(subModules.begin(), subModules.end(),
        [&](const CJMODSubModule& sub) { return sub.name == name; });
    return (it != subModules.end()) ? &(*it) : nullptr;
}

const CJMODSubModule* CJMODModule::getSubModule(const std::string& name) const {
    auto it = std::find_if(subModules.begin(), subModules.end(),
        [&](const CJMODSubModule& sub) { return sub.name == name; });
    return (it != subModules.end()) ? &(*it) : nullptr;
}

std::vector<CJMODSubModule> CJMODModule::getAllSubModules() const {
    return subModules;
}

bool CJMODModule::hasSubModule(const std::string& name) const {
    return std::any_of(subModules.begin(), subModules.end(),
        [&](const CJMODSubModule& sub) { return sub.name == name; });
}

// 依赖管理实现
void CJMODModule::addDependency(const std::string& dependency) {
    if (!hasDependency(dependency)) {
        info.dependencies.push_back(dependency);
    }
}

void CJMODModule::removeDependency(const std::string& dependency) {
    info.dependencies.erase(
        std::remove(info.dependencies.begin(), info.dependencies.end(), dependency),
        info.dependencies.end()
    );
}

std::vector<std::string> CJMODModule::getDependencies() const {
    return info.dependencies;
}

bool CJMODModule::hasDependency(const std::string& dependency) const {
    return std::find(info.dependencies.begin(), info.dependencies.end(), dependency) != info.dependencies.end();
}

bool CJMODModule::isDependencySatisfied(const std::string& dependency) const {
    // 这里可以实现依赖检查逻辑
    // 暂时返回true，实际实现需要检查依赖模块是否已加载
    return true;
}

// 元数据管理实现
void CJMODModule::setMetadata(const std::string& key, const std::string& value) {
    info.metadata[key] = value;
}

std::string CJMODModule::getMetadata(const std::string& key) const {
    auto it = info.metadata.find(key);
    return (it != info.metadata.end()) ? it->second : "";
}

bool CJMODModule::hasMetadata(const std::string& key) const {
    return info.metadata.find(key) != info.metadata.end();
}

void CJMODModule::removeMetadata(const std::string& key) {
    info.metadata.erase(key);
}

std::map<std::string, std::string> CJMODModule::getAllMetadata() const {
    return info.metadata;
}

// 扫描功能实现
CJMODScanResult CJMODModule::scanCode(const std::string& code) const {
    CJMODScanResult result;
    result.scanTime = std::chrono::system_clock::now();
    
    try {
        result.apiItems = extractAPIItems(code);
        result.imports = extractImports(code);
        result.exports = extractExports(code);
        result.metadata = extractMetadata(code);
        result.success = true;
    } catch (const std::exception& e) {
        result.errors.push_back("扫描代码失败: " + std::string(e.what()));
        result.success = false;
    }
    
    return result;
}

CJMODScanResult CJMODModule::scanFile(const std::string& filePath) const {
    CJMODScanResult result;
    result.scanTime = std::chrono::system_clock::now();
    
    try {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            result.errors.push_back("无法打开文件: " + filePath);
            result.success = false;
            return result;
        }
        
        std::string code((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
        file.close();
        
        return scanCode(code);
    } catch (const std::exception& e) {
        result.errors.push_back("扫描文件失败: " + std::string(e.what()));
        result.success = false;
        return result;
    }
}

CJMODScanResult CJMODModule::scanDirectory(const std::string& directory) const {
    CJMODScanResult result;
    result.scanTime = std::chrono::system_clock::now();
    
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".js") {
                auto fileResult = scanFile(entry.path().string());
                if (fileResult.success) {
                    result.apiItems.insert(result.apiItems.end(), 
                                          fileResult.apiItems.begin(), fileResult.apiItems.end());
                    result.imports.insert(result.imports.end(), 
                                        fileResult.imports.begin(), fileResult.imports.end());
                    result.exports.insert(result.exports.end(), 
                                        fileResult.exports.begin(), fileResult.exports.end());
                } else {
                    result.errors.insert(result.errors.end(), 
                                        fileResult.errors.begin(), fileResult.errors.end());
                }
            }
        }
        result.success = result.errors.empty();
    } catch (const std::exception& e) {
        result.errors.push_back("扫描目录失败: " + std::string(e.what()));
        result.success = false;
    }
    
    return result;
}

// 生成功能实现
std::string CJMODModule::generateCode(const CJMODGenerateConfig& config) const {
    if (config.format == "javascript") {
        return generateJavaScript();
    } else if (config.format == "typescript") {
        return generateTypeScript();
    } else if (config.format == "json") {
        return generateJSON();
    } else if (config.format == "xml") {
        return generateXML();
    } else {
        return generateJavaScript();
    }
}

bool CJMODModule::generateToFile(const CJMODGenerateConfig& config) const {
    try {
        std::ofstream file(config.outputPath);
        if (!file.is_open()) {
            return false;
        }
        
        file << generateCode(config);
        file.close();
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::string CJMODModule::generateJavaScript() const {
    std::ostringstream oss;
    
    oss << generateModuleHeader();
    
    for (const auto& item : apiItems) {
        oss << generateAPIItemCode(item) << std::endl;
    }
    
    oss << generateModuleFooter();
    
    return oss.str();
}

std::string CJMODModule::generateTypeScript() const {
    std::ostringstream oss;
    
    oss << "// TypeScript definitions for " << info.name << " v" << info.version << std::endl;
    oss << "// Generated on " << std::chrono::system_clock::now().time_since_epoch().count() << std::endl;
    oss << std::endl;
    
    for (const auto& item : apiItems) {
        if (item.isPublic) {
            oss << "export ";
            if (item.isStatic) oss << "static ";
            if (item.isAsync) oss << "async ";
            
            oss << item.name;
            if (!item.parameters.empty()) {
                oss << "(";
                for (size_t i = 0; i < item.parameters.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << item.parameters[i];
                }
                oss << ")";
            }
            
            if (!item.returnType.empty()) {
                oss << ": " << item.returnType;
            }
            
            oss << ";" << std::endl;
        }
    }
    
    return oss.str();
}

std::string CJMODModule::generateJSON() const {
    std::ostringstream oss;
    
    oss << "{" << std::endl;
    oss << "  \"name\": \"" << info.name << "\"," << std::endl;
    oss << "  \"version\": \"" << info.version << "\"," << std::endl;
    oss << "  \"description\": \"" << info.description << "\"," << std::endl;
    oss << "  \"author\": \"" << info.author << "\"," << std::endl;
    oss << "  \"license\": \"" << info.license << "\"," << std::endl;
    oss << "  \"type\": " << static_cast<int>(info.type) << "," << std::endl;
    oss << "  \"status\": " << static_cast<int>(info.status) << "," << std::endl;
    oss << "  \"apiItems\": [" << std::endl;
    
    for (size_t i = 0; i < apiItems.size(); ++i) {
        if (i > 0) oss << "," << std::endl;
        oss << "    {" << std::endl;
        oss << "      \"name\": \"" << apiItems[i].name << "\"," << std::endl;
        oss << "      \"type\": " << static_cast<int>(apiItems[i].type) << "," << std::endl;
        oss << "      \"signature\": \"" << apiItems[i].signature << "\"," << std::endl;
        oss << "      \"description\": \"" << apiItems[i].description << "\"," << std::endl;
        oss << "      \"isPublic\": " << (apiItems[i].isPublic ? "true" : "false") << "," << std::endl;
        oss << "      \"isStatic\": " << (apiItems[i].isStatic ? "true" : "false") << "," << std::endl;
        oss << "      \"isAsync\": " << (apiItems[i].isAsync ? "true" : "false") << std::endl;
        oss << "    }";
    }
    
    oss << std::endl << "  ]" << std::endl;
    oss << "}" << std::endl;
    
    return oss.str();
}

std::string CJMODModule::generateXML() const {
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
    oss << "  <apiItems>" << std::endl;
    
    for (const auto& item : apiItems) {
        oss << "    <apiItem>" << std::endl;
        oss << "      <name>" << item.name << "</name>" << std::endl;
        oss << "      <type>" << static_cast<int>(item.type) << "</type>" << std::endl;
        oss << "      <signature>" << item.signature << "</signature>" << std::endl;
        oss << "      <description>" << item.description << "</description>" << std::endl;
        oss << "      <isPublic>" << (item.isPublic ? "true" : "false") << "</isPublic>" << std::endl;
        oss << "      <isStatic>" << (item.isStatic ? "true" : "false") << "</isStatic>" << std::endl;
        oss << "      <isAsync>" << (item.isAsync ? "true" : "false") << "</isAsync>" << std::endl;
        oss << "    </apiItem>" << std::endl;
    }
    
    oss << "  </apiItems>" << std::endl;
    oss << "</module>" << std::endl;
    
    return oss.str();
}

// 模块操作实现
bool CJMODModule::load() {
    if (info.status == CJMODModuleStatus::LOADED) {
        return true;
    }
    
    if (info.status == CJMODModuleStatus::ERROR) {
        addError("模块处于错误状态，无法加载");
        return false;
    }
    
    if (info.status == CJMODModuleStatus::DISABLED) {
        addError("模块已禁用，无法加载");
        return false;
    }
    
    info.status = CJMODModuleStatus::LOADING;
    
    // 检查依赖
    for (const auto& dependency : info.dependencies) {
        if (!isDependencySatisfied(dependency)) {
            addError("依赖未满足: " + dependency);
            info.status = CJMODModuleStatus::ERROR;
            return false;
        }
    }
    
    // 验证模块
    if (!validate()) {
        info.status = CJMODModuleStatus::ERROR;
        return false;
    }
    
    info.status = CJMODModuleStatus::LOADED;
    return true;
}

bool CJMODModule::unload() {
    if (info.status != CJMODModuleStatus::LOADED) {
        return false;
    }
    
    info.status = CJMODModuleStatus::UNLOADED;
    return true;
}

bool CJMODModule::reload() {
    if (info.status == CJMODModuleStatus::LOADED) {
        if (!unload()) {
            return false;
        }
    }
    
    return load();
}

bool CJMODModule::enable() {
    if (info.status == CJMODModuleStatus::DISABLED) {
        info.status = CJMODModuleStatus::UNLOADED;
        return true;
    }
    return false;
}

bool CJMODModule::disable() {
    if (info.status == CJMODModuleStatus::LOADED) {
        if (!unload()) {
            return false;
        }
    }
    
    info.status = CJMODModuleStatus::DISABLED;
    return true;
}

// 验证实现
bool CJMODModule::validate() const {
    return validateModuleInfo() && validateAPIItems() && validateSubModules() && validateDependencies();
}

std::vector<std::string> CJMODModule::getValidationErrors() const {
    std::vector<std::string> errors;
    
    if (!validateModuleInfo()) {
        errors.push_back("模块信息验证失败");
    }
    
    if (!validateAPIItems()) {
        errors.push_back("API项验证失败");
    }
    
    if (!validateSubModules()) {
        errors.push_back("子模块验证失败");
    }
    
    if (!validateDependencies()) {
        errors.push_back("依赖验证失败");
    }
    
    return errors;
}

// 序列化实现
std::string CJMODModule::toJSON() const {
    return generateJSON();
}

bool CJMODModule::fromJSON(const std::string& json) {
    // 简单的JSON解析实现
    // 实际实现应该使用JSON库
    return true;
}

std::string CJMODModule::toXML() const {
    return generateXML();
}

bool CJMODModule::fromXML(const std::string& xml) {
    // 简单的XML解析实现
    // 实际实现应该使用XML库
    return true;
}

// 工具方法实现
std::string CJMODModule::getDisplayName() const {
    return info.name + " v" + info.version;
}

std::string CJMODModule::getVersionString() const {
    return info.version;
}

std::string CJMODModule::getTypeString() const {
    switch (static_cast<int>(info.type)) {
        case static_cast<int>(CJMODModuleType::CORE): return "Core";
        case static_cast<int>(CJMODModuleType::FEATURE): return "Feature";
        case static_cast<int>(CJMODModuleType::UTILITY): return "Utility";
        case static_cast<int>(CJMODModuleType::PLUGIN): return "Plugin";
        case static_cast<int>(CJMODModuleType::CUSTOM): return "Custom";
        default: return "Unknown";
    }
}

std::string CJMODModule::getStatusString() const {
    switch (static_cast<int>(info.status)) {
        case static_cast<int>(CJMODModuleStatus::UNLOADED): return "Unloaded";
        case static_cast<int>(CJMODModuleStatus::LOADING): return "Loading";
        case static_cast<int>(CJMODModuleStatus::LOADED): return "Loaded";
        case static_cast<int>(CJMODModuleStatus::ERROR): return "Error";
        case static_cast<int>(CJMODModuleStatus::DISABLED): return "Disabled";
        default: return "Unknown";
    }
}

bool CJMODModule::isCompatible(const std::string& version) const {
    return compareVersion(version) >= 0;
}

int CJMODModule::compareVersion(const std::string& version) const {
    return compareVersionNumbers(info.version, version);
}

// 错误处理实现
std::vector<std::string> CJMODModule::getErrors() const {
    return errors;
}

std::vector<std::string> CJMODModule::getWarnings() const {
    return warnings;
}

void CJMODModule::addError(const std::string& error) {
    errors.push_back(error);
}

void CJMODModule::addWarning(const std::string& warning) {
    warnings.push_back(warning);
}

void CJMODModule::clearErrors() {
    errors.clear();
}

void CJMODModule::clearWarnings() {
    warnings.clear();
}

// 内部方法实现
bool CJMODModule::validateModuleInfo() const {
    return !info.name.empty() && !info.version.empty();
}

bool CJMODModule::validateAPIItems() const {
    // 检查API项名称是否唯一
    std::set<std::string> apiNames;
    for (const auto& item : apiItems) {
        if (apiNames.find(item.name) != apiNames.end()) {
            return false; // 重复的API项名称
        }
        apiNames.insert(item.name);
    }
    return true;
}

bool CJMODModule::validateSubModules() const {
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

bool CJMODModule::validateDependencies() const {
    // 检查依赖是否有效
    for (const auto& dep : info.dependencies) {
        if (dep.empty()) {
            return false;
        }
    }
    return true;
}

std::string CJMODModule::generateChecksum() const {
    // 简单的校验和生成
    std::string data = info.name + info.version + info.description;
    std::hash<std::string> hasher;
    return std::to_string(hasher(data));
}

bool CJMODModule::verifyChecksum() const {
    return info.checksum == generateChecksum();
}

void CJMODModule::updateLastModified() {
    info.lastModified = std::chrono::system_clock::now();
}

int CJMODModule::compareVersionNumbers(const std::string& v1, const std::string& v2) const {
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

std::vector<int> CJMODModule::parseVersion(const std::string& version) const {
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

// 扫描辅助方法实现
std::vector<CJMODAPIItem> CJMODModule::extractAPIItems(const std::string& code) const {
    std::vector<CJMODAPIItem> items;
    
    // 简单的正则表达式匹配来提取API项
    // 实际实现应该使用更复杂的解析器
    
    // 匹配函数声明
    std::regex functionRegex(R"(function\s+(\w+)\s*\([^)]*\)\s*\{)");
    std::sregex_iterator iter(code.begin(), code.end(), functionRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        CJMODAPIItem item;
        item.name = (*iter)[1].str();
        item.type = CJMODAPIType::FUNCTION;
        item.signature = (*iter)[0].str();
        item.isPublic = true;
        item.isStatic = false;
        item.isAsync = false;
        items.push_back(item);
    }
    
    // 匹配类声明
    std::regex classRegex(R"(class\s+(\w+)\s*(?:extends\s+\w+)?\s*\{)");
    iter = std::sregex_iterator(code.begin(), code.end(), classRegex);
    
    for (; iter != end; ++iter) {
        CJMODAPIItem item;
        item.name = (*iter)[1].str();
        item.type = CJMODAPIType::CLASS;
        item.signature = (*iter)[0].str();
        item.isPublic = true;
        item.isStatic = false;
        item.isAsync = false;
        items.push_back(item);
    }
    
    return items;
}

std::vector<std::string> CJMODModule::extractImports(const std::string& code) const {
    std::vector<std::string> imports;
    
    // 匹配import语句
    std::regex importRegex(R"(import\s+.*?from\s+['"]([^'"]+)['"])");
    std::sregex_iterator iter(code.begin(), code.end(), importRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        imports.push_back((*iter)[1].str());
    }
    
    return imports;
}

std::vector<std::string> CJMODModule::extractExports(const std::string& code) const {
    std::vector<std::string> exports;
    
    // 匹配export语句
    std::regex exportRegex(R"(export\s+(?:default\s+)?(?:function\s+)?(\w+))");
    std::sregex_iterator iter(code.begin(), code.end(), exportRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        exports.push_back((*iter)[1].str());
    }
    
    return exports;
}

std::map<std::string, std::string> CJMODModule::extractMetadata(const std::string& code) const {
    std::map<std::string, std::string> metadata;
    
    // 匹配注释中的元数据
    std::regex metadataRegex(R"(//\s*@(\w+)\s+(.+))");
    std::sregex_iterator iter(code.begin(), code.end(), metadataRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        metadata[(*iter)[1].str()] = (*iter)[2].str();
    }
    
    return metadata;
}

// 生成辅助方法实现
std::string CJMODModule::generateAPIItemCode(const CJMODAPIItem& item) const {
    std::ostringstream oss;
    
    if (item.isPublic) {
        oss << "export ";
    }
    
    if (item.isStatic) {
        oss << "static ";
    }
    
    if (item.isAsync) {
        oss << "async ";
    }
    
    switch (item.type) {
        case CJMODAPIType::FUNCTION:
            oss << "function " << item.name;
            break;
        case CJMODAPIType::CLASS:
            oss << "class " << item.name;
            break;
        case CJMODAPIType::INTERFACE:
            oss << "interface " << item.name;
            break;
        case CJMODAPIType::ENUM:
            oss << "enum " << item.name;
            break;
        case CJMODAPIType::CONSTANT:
            oss << "const " << item.name;
            break;
        case CJMODAPIType::VARIABLE:
            oss << "let " << item.name;
            break;
        case CJMODAPIType::NAMESPACE:
            oss << "namespace " << item.name;
            break;
    }
    
    if (!item.signature.empty()) {
        oss << " " << item.signature;
    }
    
    return oss.str();
}

std::string CJMODModule::generateModuleHeader() const {
    std::ostringstream oss;
    
    oss << "// " << info.name << " v" << info.version << std::endl;
    oss << "// " << info.description << std::endl;
    oss << "// Author: " << info.author << std::endl;
    oss << "// License: " << info.license << std::endl;
    oss << "// Generated on " << std::chrono::system_clock::now().time_since_epoch().count() << std::endl;
    oss << std::endl;
    
    return oss.str();
}

std::string CJMODModule::generateModuleFooter() const {
    std::ostringstream oss;
    
    oss << std::endl;
    oss << "// End of " << info.name << " module" << std::endl;
    
    return oss.str();
}

} // namespace Module
} // namespace CHTL