#include "ModuleSystem.h"
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>
#include <filesystem>

namespace CHTL {

// ExportTable 实现
void ExportTable::addExport(const ExportItem& item) {
    exports_[item.name] = item;
}

void ExportTable::addExport(const std::string& name, const std::string& type, bool isPublic) {
    exports_[name] = ExportItem(name, type, isPublic);
}

void ExportTable::removeExport(const std::string& name) {
    exports_.erase(name);
}

ExportItem ExportTable::getExport(const std::string& name) const {
    auto it = exports_.find(name);
    return (it != exports_.end()) ? it->second : ExportItem();
}

bool ExportTable::hasExport(const std::string& name) const {
    return exports_.find(name) != exports_.end();
}

std::vector<ExportItem> ExportTable::getAllExports() const {
    std::vector<ExportItem> result;
    for (const auto& pair : exports_) {
        result.push_back(pair.second);
    }
    return result;
}

std::vector<ExportItem> ExportTable::getPublicExports() const {
    std::vector<ExportItem> result;
    for (const auto& pair : exports_) {
        if (pair.second.isPublic) {
            result.push_back(pair.second);
        }
    }
    return result;
}

void ExportTable::clear() {
    exports_.clear();
}

// Module 实现
Module::Module(ModuleType type, const std::string& name)
    : type_(type), name_(name) {
    exportTable_ = std::make_shared<ExportTable>();
}

void Module::setInfo(const ModuleInfo& info) {
    info_ = info;
}

void Module::setExportTable(std::shared_ptr<ExportTable> exportTable) {
    exportTable_ = exportTable;
}

void Module::setContent(const std::string& content) {
    content_ = content;
}

void Module::setPath(const std::string& path) {
    path_ = path;
}

void Module::addDependency(const std::string& dependency) {
    if (!hasDependency(dependency)) {
        dependencies_.push_back(dependency);
    }
}

bool Module::hasDependency(const std::string& dependency) const {
    return std::find(dependencies_.begin(), dependencies_.end(), dependency) != dependencies_.end();
}

bool Module::isValid() const {
    // 验证模块基本信息
    if (name_.empty()) {
        return false;
    }
    
    // 验证模块信息
    if (info_.name.empty() || info_.version.empty()) {
        return false;
    }
    
    // 验证导出表
    if (!exportTable_) {
        return false;
    }
    
    // 验证内容
    if (content_.empty()) {
        return false;
    }
    
    return true;
}

// ModuleSystem 实现
ModuleSystem::ModuleSystem() {
    // 添加默认模块路径
    modulePaths_.push_back("./modules");
    modulePaths_.push_back("./node_modules");
    modulePaths_.push_back("/usr/local/lib/chtl/modules");
    modulePaths_.push_back("/usr/lib/chtl/modules");
}

void ModuleSystem::registerModule(std::shared_ptr<Module> module) {
    if (module && module->isValid()) {
        modules_[module->getName()] = module;
    }
}

std::shared_ptr<Module> ModuleSystem::getModule(const std::string& name) const {
    auto it = modules_.find(name);
    return (it != modules_.end()) ? it->second : nullptr;
}

bool ModuleSystem::hasModule(const std::string& name) const {
    return modules_.find(name) != modules_.end();
}

void ModuleSystem::removeModule(const std::string& name) {
    modules_.erase(name);
}

std::shared_ptr<Module> ModuleSystem::loadModule(const std::string& path) {
    if (!fileExists(path)) {
        reportError("Module file not found: " + path);
        return nullptr;
    }
    
    std::string content = readFile(path);
    if (content.empty()) {
        reportError("Failed to read module file: " + path);
        return nullptr;
    }
    
    return parseModule(content, path);
}

std::shared_ptr<Module> ModuleSystem::loadCMOD(const std::string& path) {
    auto module = loadModule(path);
    if (module && module->getType() == ModuleType::CMOD) {
        return module;
    }
    return nullptr;
}

std::shared_ptr<Module> ModuleSystem::loadCJMOD(const std::string& path) {
    auto module = loadModule(path);
    if (module && module->getType() == ModuleType::CJMOD) {
        return module;
    }
    return nullptr;
}

std::shared_ptr<Module> ModuleSystem::loadMixedModule(const std::string& path) {
    auto module = loadModule(path);
    if (module && module->getType() == ModuleType::MIXED) {
        return module;
    }
    return nullptr;
}

std::shared_ptr<Module> ModuleSystem::parseModule(const std::string& content, const std::string& path) {
    // 解析模块类型
    ModuleType type = ModuleType::UNKNOWN;
    if (content.find("[Info]") != std::string::npos && content.find("[Export]") != std::string::npos) {
        type = ModuleType::CMOD;
    } else if (content.find("[Info]") != std::string::npos && content.find("[Export]") == std::string::npos) {
        type = ModuleType::CJMOD;
    } else if (content.find("[Info]") != std::string::npos) {
        type = ModuleType::MIXED;
    }
    
    if (type == ModuleType::UNKNOWN) {
        reportError("Unknown module type");
        return nullptr;
    }
    
    // 解析模块信息
    ModuleInfo info = parseModuleInfo(content);
    if (info.name.empty()) {
        reportError("Module name not found");
        return nullptr;
    }
    
    // 创建模块
    auto module = std::make_shared<Module>(type, info.name);
    module->setInfo(info);
    module->setPath(path);
    
    // 解析导出表
    if (type == ModuleType::CMOD || type == ModuleType::MIXED) {
        auto exportTable = parseExportTable(content);
        if (exportTable) {
            module->setExportTable(exportTable);
        }
    }
    
    // 解析模块内容
    std::string moduleContent = extractModuleContent(content);
    module->setContent(moduleContent);
    
    // 处理依赖
    processDependencies(module);
    
    return module;
}

ModuleInfo ModuleSystem::parseModuleInfo(const std::string& content) {
    ModuleInfo info;
    
    // 提取 [Info] 块
    std::string infoBlock = extractModuleInfo(content);
    if (infoBlock.empty()) {
        return info;
    }
    
    // 解析信息字段
    std::regex nameRegex(R"(name\s*:\s*([^\n]+))");
    std::regex versionRegex(R"(version\s*:\s*([^\n]+))");
    std::regex descriptionRegex(R"(description\s*:\s*([^\n]+))");
    std::regex authorRegex(R"(author\s*:\s*([^\n]+))");
    std::regex licenseRegex(R"(license\s*:\s*([^\n]+))");
    std::regex repositoryRegex(R"(repository\s*:\s*([^\n]+))");
    std::regex homepageRegex(R"(homepage\s*:\s*([^\n]+))");
    
    std::smatch match;
    
    if (std::regex_search(infoBlock, match, nameRegex)) {
        info.name = match[1].str();
    }
    if (std::regex_search(infoBlock, match, versionRegex)) {
        info.version = match[1].str();
    }
    if (std::regex_search(infoBlock, match, descriptionRegex)) {
        info.description = match[1].str();
    }
    if (std::regex_search(infoBlock, match, authorRegex)) {
        info.author = match[1].str();
    }
    if (std::regex_search(infoBlock, match, licenseRegex)) {
        info.license = match[1].str();
    }
    if (std::regex_search(infoBlock, match, repositoryRegex)) {
        info.repository = match[1].str();
    }
    if (std::regex_search(infoBlock, match, homepageRegex)) {
        info.homepage = match[1].str();
    }
    
    // 解析依赖
    std::regex dependencyRegex(R"(dependency\s*:\s*([^\n]+))");
    std::string::const_iterator start = infoBlock.begin();
    while (std::regex_search(start, infoBlock.end(), match, dependencyRegex)) {
        info.dependencies.push_back(match[1].str());
        start = match[0].second;
    }
    
    // 解析关键字
    std::regex keywordRegex(R"(keyword\s*:\s*([^\n]+))");
    start = infoBlock.begin();
    while (std::regex_search(start, infoBlock.end(), match, keywordRegex)) {
        info.keywords.push_back(match[1].str());
        start = match[0].second;
    }
    
    return info;
}

std::shared_ptr<ExportTable> ModuleSystem::parseExportTable(const std::string& content) {
    auto exportTable = std::make_shared<ExportTable>();
    
    // 提取 [Export] 块
    std::string exportBlock = extractExportTable(content);
    if (exportBlock.empty()) {
        return exportTable;
    }
    
    // 解析导出项
    std::regex exportRegex(R"(\w+\s*:\s*(\w+)\s*:\s*([^\n]+))");
    std::string::const_iterator start = exportBlock.begin();
    std::smatch match;
    
    while (std::regex_search(start, exportBlock.end(), match, exportRegex)) {
        std::string name = match[0].str().substr(0, match[0].str().find(':'));
        std::string type = match[1].str();
        std::string description = match[2].str();
        
        // 检查是否是公共导出
        bool isPublic = (type == "public");
        
        exportTable->addExport(name, type, isPublic);
        start = match[0].second;
    }
    
    return exportTable;
}

std::vector<std::string> ModuleSystem::searchModules(const std::string& query) const {
    std::vector<std::string> result;
    
    for (const auto& pair : modules_) {
        const auto& module = pair.second;
        const auto& info = module->getInfo();
        
        // 搜索名称
        if (info.name.find(query) != std::string::npos) {
            result.push_back(info.name);
            continue;
        }
        
        // 搜索描述
        if (info.description.find(query) != std::string::npos) {
            result.push_back(info.name);
            continue;
        }
        
        // 搜索关键字
        for (const auto& keyword : info.keywords) {
            if (keyword.find(query) != std::string::npos) {
                result.push_back(info.name);
                break;
            }
        }
    }
    
    return result;
}

std::vector<std::string> ModuleSystem::searchModulesByType(ModuleType type) const {
    std::vector<std::string> result;
    
    for (const auto& pair : modules_) {
        if (pair.second->getType() == type) {
            result.push_back(pair.first);
        }
    }
    
    return result;
}

std::vector<std::string> ModuleSystem::searchModulesByKeyword(const std::string& keyword) const {
    std::vector<std::string> result;
    
    for (const auto& pair : modules_) {
        const auto& info = pair.second->getInfo();
        
        for (const auto& kw : info.keywords) {
            if (kw == keyword) {
                result.push_back(pair.first);
                break;
            }
        }
    }
    
    return result;
}

std::vector<std::string> ModuleSystem::resolveDependencies(const std::string& moduleName) const {
    std::vector<std::string> result;
    std::unordered_set<std::string> visited;
    
    auto module = getModule(moduleName);
    if (!module) {
        return result;
    }
    
    // 递归解析依赖
    std::function<void(const std::string&)> resolve = [&](const std::string& name) {
        if (visited.find(name) != visited.end()) {
            return; // 避免循环依赖
        }
        
        visited.insert(name);
        auto depModule = getModule(name);
        if (depModule) {
            for (const auto& dep : depModule->getDependencies()) {
                resolve(dep);
            }
            result.push_back(name);
        }
    };
    
    resolve(moduleName);
    return result;
}

bool ModuleSystem::checkDependencies(const std::string& moduleName) const {
    auto module = getModule(moduleName);
    if (!module) {
        return false;
    }
    
    for (const auto& dependency : module->getDependencies()) {
        if (!hasModule(dependency)) {
            return false;
        }
    }
    
    return true;
}

void ModuleSystem::addModulePath(const std::string& path) {
    if (std::find(modulePaths_.begin(), modulePaths_.end(), path) == modulePaths_.end()) {
        modulePaths_.push_back(path);
    }
}

std::string ModuleSystem::findModule(const std::string& name) const {
    // 检查官方模块
    auto it = officialModules_.find(name);
    if (it != officialModules_.end()) {
        return it->second;
    }
    
    // 在模块路径中搜索
    for (const auto& path : modulePaths_) {
        std::string fullPath = path + "/" + name;
        if (fileExists(fullPath)) {
            return fullPath;
        }
    }
    
    return "";
}

void ModuleSystem::addOfficialModule(const std::string& name, const std::string& path) {
    officialModules_[name] = path;
}

std::string ModuleSystem::getOfficialModule(const std::string& name) const {
    auto it = officialModules_.find(name);
    return (it != officialModules_.end()) ? it->second : "";
}

bool ModuleSystem::isOfficialModule(const std::string& name) const {
    return officialModules_.find(name) != officialModules_.end();
}

void ModuleSystem::setErrorHandler(std::function<void(const std::string&, size_t, size_t)> handler) {
    errorHandler_ = handler;
}

void ModuleSystem::clear() {
    modules_.clear();
    modulePaths_.clear();
    officialModules_.clear();
}

void ModuleSystem::reportError(const std::string& message, size_t line, size_t column) {
    if (errorHandler_) {
        errorHandler_(message, line, column);
    } else {
        std::cerr << "Module System Error at " << line << ":" << column << ": " << message << std::endl;
    }
}

std::string ModuleSystem::extractModuleInfo(const std::string& content) const {
    std::regex infoRegex(R"(\[Info\]\s*\{([^}]+)\})");
    std::smatch match;
    
    if (std::regex_search(content, match, infoRegex)) {
        return match[1].str();
    }
    
    return "";
}

std::string ModuleSystem::extractExportTable(const std::string& content) const {
    std::regex exportRegex(R"(\[Export\]\s*\{([^}]+)\})");
    std::smatch match;
    
    if (std::regex_search(content, match, exportRegex)) {
        return match[1].str();
    }
    
    return "";
}

std::string ModuleSystem::extractModuleContent(const std::string& content) const {
    // 提取模块内容（排除 [Info] 和 [Export] 块）
    std::string result = content;
    
    // 移除 [Info] 块
    std::regex infoRegex(R"(\[Info\]\s*\{[^}]+\})");
    result = std::regex_replace(result, infoRegex, "");
    
    // 移除 [Export] 块
    std::regex exportRegex(R"(\[Export\]\s*\{[^}]+\})");
    result = std::regex_replace(result, exportRegex, "");
    
    return result;
}

std::string ModuleSystem::resolveModulePath(const std::string& name) const {
    return findModule(name);
}

std::string ModuleSystem::getModuleDirectory(const std::string& path) const {
    return std::filesystem::path(path).parent_path().string();
}

std::string ModuleSystem::getModuleFileName(const std::string& path) const {
    return std::filesystem::path(path).filename().string();
}

bool ModuleSystem::fileExists(const std::string& path) const {
    return std::filesystem::exists(path);
}

std::string ModuleSystem::readFile(const std::string& path) const {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }
    
    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool ModuleSystem::writeFile(const std::string& path, const std::string& content) const {
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return true;
}

void ModuleSystem::processDependencies(std::shared_ptr<Module> module) {
    if (!module) {
        return;
    }
    
    const auto& info = module->getInfo();
    for (const auto& dependency : info.dependencies) {
        module->addDependency(dependency);
    }
}

bool ModuleSystem::isDependencyResolved(const std::string& dependency) const {
    return hasModule(dependency);
}

bool ModuleSystem::validateModule(std::shared_ptr<Module> module) const {
    if (!module) {
        return false;
    }
    
    return module->isValid() && validateModuleInfo(module->getInfo()) && 
           validateExportTable(module->getExportTable());
}

bool ModuleSystem::validateModuleInfo(const ModuleInfo& info) const {
    return !info.name.empty() && !info.version.empty();
}

bool ModuleSystem::validateExportTable(std::shared_ptr<ExportTable> exportTable) const {
    return exportTable != nullptr;
}

} // namespace CHTL