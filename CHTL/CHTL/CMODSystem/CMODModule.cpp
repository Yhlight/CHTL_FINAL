#include "CMODModule.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <regex>

namespace CHTL {

// CMODModule 实现
CMODModule::CMODModule() 
    : loaded(false), enabled(true) {
    info.type = ModuleType::UNKNOWN;
    info.status = ModuleStatus::UNLOADED;
}

CMODModule::CMODModule(const std::string& name, const std::string& version) 
    : loaded(false), enabled(true) {
    info.name = name;
    info.version = version;
    info.type = ModuleType::UNKNOWN;
    info.status = ModuleStatus::UNLOADED;
}

CMODModule::CMODModule(const ModuleInfo& info) 
    : info(info), loaded(false), enabled(true) {
}

CMODModule::~CMODModule() = default;

// 基本信息实现
void CMODModule::setName(const std::string& name) {
    info.name = name;
}

std::string CMODModule::getName() const {
    return info.name;
}

void CMODModule::setVersion(const std::string& version) {
    info.version = version;
}

std::string CMODModule::getVersion() const {
    return info.version;
}

void CMODModule::setDescription(const std::string& description) {
    info.description = description;
}

std::string CMODModule::getDescription() const {
    return info.description;
}

void CMODModule::setAuthor(const std::string& author) {
    info.author = author;
}

std::string CMODModule::getAuthor() const {
    return info.author;
}

void CMODModule::setLicense(const std::string& license) {
    info.license = license;
}

std::string CMODModule::getLicense() const {
    return info.license;
}

void CMODModule::setHomepage(const std::string& homepage) {
    info.homepage = homepage;
}

std::string CMODModule::getHomepage() const {
    return info.homepage;
}

// 模块信息实现
void CMODModule::setInfo(const ModuleInfo& info) {
    this->info = info;
}

ModuleInfo CMODModule::getInfo() const {
    return info;
}

void CMODModule::setType(ModuleType type) {
    info.type = type;
}

ModuleType CMODModule::getType() const {
    return info.type;
}

void CMODModule::setStatus(ModuleStatus status) {
    info.status = status;
}

ModuleStatus CMODModule::getStatus() const {
    return info.status;
}

void CMODModule::setMainFile(const std::string& mainFile) {
    info.mainFile = mainFile;
}

std::string CMODModule::getMainFile() const {
    return info.mainFile;
}

void CMODModule::setEntryPoint(const std::string& entryPoint) {
    info.entryPoint = entryPoint;
}

std::string CMODModule::getEntryPoint() const {
    return info.entryPoint;
}

// 关键词管理实现
void CMODModule::addKeyword(const std::string& keyword) {
    if (std::find(info.keywords.begin(), info.keywords.end(), keyword) == info.keywords.end()) {
        info.keywords.push_back(keyword);
    }
}

void CMODModule::removeKeyword(const std::string& keyword) {
    info.keywords.erase(std::remove(info.keywords.begin(), info.keywords.end(), keyword), info.keywords.end());
}

void CMODModule::clearKeywords() {
    info.keywords.clear();
}

std::vector<std::string> CMODModule::getKeywords() const {
    return info.keywords;
}

bool CMODModule::hasKeyword(const std::string& keyword) const {
    return std::find(info.keywords.begin(), info.keywords.end(), keyword) != info.keywords.end();
}

// 依赖管理实现
void CMODModule::addDependency(const ModuleDependency& dependency) {
    // 检查是否已存在同名依赖
    auto it = std::find_if(info.dependencies.begin(), info.dependencies.end(),
        [&dependency](const ModuleDependency& dep) {
            return dep.name == dependency.name;
        });
    
    if (it != info.dependencies.end()) {
        *it = dependency; // 更新现有依赖
    } else {
        info.dependencies.push_back(dependency); // 添加新依赖
    }
}

void CMODModule::addDependency(const std::string& name, const std::string& version, bool required) {
    ModuleDependency dependency(name, version, required);
    addDependency(dependency);
}

void CMODModule::removeDependency(const std::string& name) {
    info.dependencies.erase(std::remove_if(info.dependencies.begin(), info.dependencies.end(),
        [&name](const ModuleDependency& dep) {
            return dep.name == name;
        }), info.dependencies.end());
}

void CMODModule::clearDependencies() {
    info.dependencies.clear();
}

std::vector<ModuleDependency> CMODModule::getDependencies() const {
    return info.dependencies;
}

std::vector<ModuleDependency> CMODModule::getRequiredDependencies() const {
    std::vector<ModuleDependency> required;
    std::copy_if(info.dependencies.begin(), info.dependencies.end(), std::back_inserter(required),
        [](const ModuleDependency& dep) {
            return dep.required;
        });
    return required;
}

std::vector<ModuleDependency> CMODModule::getOptionalDependencies() const {
    std::vector<ModuleDependency> optional;
    std::copy_if(info.dependencies.begin(), info.dependencies.end(), std::back_inserter(optional),
        [](const ModuleDependency& dep) {
            return dep.optional;
        });
    return optional;
}

bool CMODModule::hasDependency(const std::string& name) const {
    return std::any_of(info.dependencies.begin(), info.dependencies.end(),
        [&name](const ModuleDependency& dep) {
            return dep.name == name;
        });
}

ModuleDependency CMODModule::getDependency(const std::string& name) const {
    auto it = std::find_if(info.dependencies.begin(), info.dependencies.end(),
        [&name](const ModuleDependency& dep) {
            return dep.name == name;
        });
    
    if (it != info.dependencies.end()) {
        return *it;
    }
    
    return ModuleDependency(); // 返回空依赖
}

// 导出管理实现
void CMODModule::addExport(const ModuleExport& export_) {
    // 检查是否已存在同名导出
    auto it = std::find_if(info.exports.begin(), info.exports.end(),
        [&export_](const ModuleExport& exp) {
            return exp.name == export_.name;
        });
    
    if (it != info.exports.end()) {
        *it = export_; // 更新现有导出
    } else {
        info.exports.push_back(export_); // 添加新导出
    }
}

void CMODModule::addExport(const std::string& name, const std::any& value, const std::string& type) {
    ModuleExport export_(name, value, type);
    addExport(export_);
}

void CMODModule::addDefaultExport(const std::any& value, const std::string& type) {
    ModuleExport export_("default", value, type);
    export_.isDefault = true;
    addExport(export_);
}

void CMODModule::removeExport(const std::string& name) {
    info.exports.erase(std::remove_if(info.exports.begin(), info.exports.end(),
        [&name](const ModuleExport& exp) {
            return exp.name == name;
        }), info.exports.end());
}

void CMODModule::clearExports() {
    info.exports.clear();
}

std::vector<ModuleExport> CMODModule::getExports() const {
    return info.exports;
}

std::vector<ModuleExport> CMODModule::getNamedExports() const {
    std::vector<ModuleExport> named;
    std::copy_if(info.exports.begin(), info.exports.end(), std::back_inserter(named),
        [](const ModuleExport& exp) {
            return !exp.isDefault;
        });
    return named;
}

ModuleExport CMODModule::getDefaultExport() const {
    auto it = std::find_if(info.exports.begin(), info.exports.end(),
        [](const ModuleExport& exp) {
            return exp.isDefault;
        });
    
    if (it != info.exports.end()) {
        return *it;
    }
    
    return ModuleExport(); // 返回空导出
}

bool CMODModule::hasExport(const std::string& name) const {
    return std::any_of(info.exports.begin(), info.exports.end(),
        [&name](const ModuleExport& exp) {
            return exp.name == name;
        });
}

ModuleExport CMODModule::getExport(const std::string& name) const {
    auto it = std::find_if(info.exports.begin(), info.exports.end(),
        [&name](const ModuleExport& exp) {
            return exp.name == name;
        });
    
    if (it != info.exports.end()) {
        return *it;
    }
    
    return ModuleExport(); // 返回空导出
}

std::any CMODModule::getExportValue(const std::string& name) const {
    ModuleExport export_ = getExport(name);
    return export_.value;
}

// 元数据管理实现
void CMODModule::setMetadata(const std::map<std::string, std::any>& metadata) {
    info.metadata = metadata;
}

std::map<std::string, std::any> CMODModule::getMetadata() const {
    return info.metadata;
}

void CMODModule::addMetadata(const std::string& key, const std::any& value) {
    info.metadata[key] = value;
}

std::any CMODModule::getMetadata(const std::string& key) const {
    auto it = info.metadata.find(key);
    return it != info.metadata.end() ? it->second : std::any{};
}

bool CMODModule::hasMetadata(const std::string& key) const {
    return info.metadata.find(key) != info.metadata.end();
}

void CMODModule::removeMetadata(const std::string& key) {
    info.metadata.erase(key);
}

void CMODModule::clearMetadata() {
    info.metadata.clear();
}

// 子模块管理实现
void CMODModule::addSubmodule(std::shared_ptr<CMODModule> submodule) {
    if (submodule) {
        submodules.push_back(submodule);
    }
}

void CMODModule::removeSubmodule(const std::string& name) {
    submodules.erase(std::remove_if(submodules.begin(), submodules.end(),
        [&name](const std::shared_ptr<CMODModule>& submodule) {
            return submodule && submodule->getName() == name;
        }), submodules.end());
}

void CMODModule::clearSubmodules() {
    submodules.clear();
}

std::vector<std::shared_ptr<CMODModule>> CMODModule::getSubmodules() const {
    return submodules;
}

std::shared_ptr<CMODModule> CMODModule::getSubmodule(const std::string& name) const {
    auto it = std::find_if(submodules.begin(), submodules.end(),
        [&name](const std::shared_ptr<CMODModule>& submodule) {
            return submodule && submodule->getName() == name;
        });
    
    return it != submodules.end() ? *it : nullptr;
}

bool CMODModule::hasSubmodule(const std::string& name) const {
    return getSubmodule(name) != nullptr;
}

// 路径管理实现
void CMODModule::setPath(const std::string& path) {
    this->path = path;
}

std::string CMODModule::getPath() const {
    return path;
}

void CMODModule::addSearchPath(const std::string& path) {
    if (std::find(searchPaths.begin(), searchPaths.end(), path) == searchPaths.end()) {
        searchPaths.push_back(path);
    }
}

void CMODModule::removeSearchPath(const std::string& path) {
    searchPaths.erase(std::remove(searchPaths.begin(), searchPaths.end(), path), searchPaths.end());
}

void CMODModule::clearSearchPaths() {
    searchPaths.clear();
}

std::vector<std::string> CMODModule::getSearchPaths() const {
    return searchPaths;
}

// 加载状态实现
void CMODModule::setLoaded(bool loaded) {
    this->loaded = loaded;
    info.status = loaded ? ModuleStatus::LOADED : ModuleStatus::UNLOADED;
}

bool CMODModule::isLoaded() const {
    return loaded;
}

void CMODModule::setEnabled(bool enabled) {
    this->enabled = enabled;
}

bool CMODModule::isEnabled() const {
    return enabled;
}

void CMODModule::setError(const std::string& error) {
    this->error = error;
    info.status = ModuleStatus::ERROR;
}

std::string CMODModule::getError() const {
    return error;
}

bool CMODModule::hasError() const {
    return !error.empty();
}

void CMODModule::clearError() {
    error.clear();
    if (info.status == ModuleStatus::ERROR) {
        info.status = ModuleStatus::UNLOADED;
    }
}

// 验证实现
bool CMODModule::isValid() const {
    return validateModule() && validateInfo() && validateDependencies() && 
           validateExports() && validateSubmodules() && validateMetadata();
}

bool CMODModule::isComplete() const {
    return !info.name.empty() && !info.version.empty() && info.type != ModuleType::UNKNOWN;
}

std::vector<std::string> CMODModule::validate() const {
    std::vector<std::string> errors;
    
    if (!validateModule()) {
        errors.push_back("Module validation failed");
    }
    
    if (!validateInfo()) {
        errors.push_back("Info validation failed");
    }
    
    if (!validateDependencies()) {
        errors.push_back("Dependencies validation failed");
    }
    
    if (!validateExports()) {
        errors.push_back("Exports validation failed");
    }
    
    if (!validateSubmodules()) {
        errors.push_back("Submodules validation failed");
    }
    
    if (!validateMetadata()) {
        errors.push_back("Metadata validation failed");
    }
    
    return errors;
}

// 比较实现
bool CMODModule::equals(std::shared_ptr<CMODModule> other) const {
    return other && compareModule(other);
}

bool CMODModule::equals(const CMODModule& other) const {
    return compareModule(other);
}

// 克隆实现
std::shared_ptr<CMODModule> CMODModule::clone() const {
    auto cloned = std::make_shared<CMODModule>(info);
    copyTo(cloned);
    return cloned;
}

std::shared_ptr<CMODModule> CMODModule::deepClone() const {
    auto cloned = std::make_shared<CMODModule>(info);
    deepCopyTo(cloned);
    return cloned;
}

// 转换实现
std::string CMODModule::toJSON() const {
    return generateJSON();
}

std::string CMODModule::toXML() const {
    return generateXML();
}

std::string CMODModule::toYAML() const {
    return generateYAML();
}

std::string CMODModule::toString() const {
    return info.name + "@" + info.version;
}

std::string CMODModule::toDebugString() const {
    std::ostringstream oss;
    oss << "CMODModule{name='" << info.name
        << "', version='" << info.version
        << "', type=" << static_cast<int>(info.type)
        << ", status=" << static_cast<int>(info.status)
        << ", loaded=" << loaded
        << ", enabled=" << enabled << "}";
    return oss.str();
}

// 格式化实现
std::string CMODModule::format() const {
    return formatModule();
}

std::string CMODModule::minify() const {
    return minifyModule();
}

std::string CMODModule::beautify() const {
    return beautifyModule();
}

// 统计实现
size_t CMODModule::getDependencyCount() const {
    return info.dependencies.size();
}

size_t CMODModule::getExportCount() const {
    return info.exports.size();
}

size_t CMODModule::getSubmoduleCount() const {
    return submodules.size();
}

size_t CMODModule::getMetadataCount() const {
    return info.metadata.size();
}

// 重置实现
void CMODModule::reset() {
    info = ModuleInfo();
    path.clear();
    searchPaths.clear();
    submodules.clear();
    error.clear();
    loaded = false;
    enabled = true;
}

// 验证辅助实现
bool CMODModule::validateModule() const {
    return !info.name.empty() && !info.version.empty();
}

bool CMODModule::validateInfo() const {
    return info.type != ModuleType::UNKNOWN;
}

bool CMODModule::validateDependencies() const {
    // 简化的依赖验证
    return true;
}

bool CMODModule::validateExports() const {
    // 简化的导出验证
    return true;
}

bool CMODModule::validateSubmodules() const {
    for (const auto& submodule : submodules) {
        if (!submodule || !submodule->isValid()) {
            return false;
        }
    }
    return true;
}

bool CMODModule::validateMetadata() const {
    // 简化的元数据验证
    return true;
}

// 比较辅助实现
bool CMODModule::compareModule(std::shared_ptr<CMODModule> other) const {
    return compareModule(*other);
}

bool CMODModule::compareModule(const CMODModule& other) const {
    return compareInfo(other) && compareDependencies(other) && 
           compareExports(other) && compareSubmodules(other) && compareMetadata(other);
}

bool CMODModule::compareInfo(std::shared_ptr<CMODModule> other) const {
    return compareInfo(*other);
}

bool CMODModule::compareInfo(const CMODModule& other) const {
    return info.name == other.info.name &&
           info.version == other.info.version &&
           info.description == other.info.description &&
           info.author == other.info.author &&
           info.license == other.info.license &&
           info.homepage == other.info.homepage &&
           info.type == other.info.type &&
           info.status == other.info.status &&
           info.mainFile == other.info.mainFile &&
           info.entryPoint == other.info.entryPoint;
}

bool CMODModule::compareDependencies(std::shared_ptr<CMODModule> other) const {
    return compareDependencies(*other);
}

bool CMODModule::compareDependencies(const CMODModule& other) const {
    if (info.dependencies.size() != other.info.dependencies.size()) {
        return false;
    }
    
    for (size_t i = 0; i < info.dependencies.size(); ++i) {
        const auto& dep1 = info.dependencies[i];
        const auto& dep2 = other.info.dependencies[i];
        
        if (dep1.name != dep2.name || dep1.version != dep2.version ||
            dep1.required != dep2.required || dep1.optional != dep2.optional) {
            return false;
        }
    }
    
    return true;
}

bool CMODModule::compareExports(std::shared_ptr<CMODModule> other) const {
    return compareExports(*other);
}

bool CMODModule::compareExports(const CMODModule& other) const {
    if (info.exports.size() != other.info.exports.size()) {
        return false;
    }
    
    for (size_t i = 0; i < info.exports.size(); ++i) {
        const auto& exp1 = info.exports[i];
        const auto& exp2 = other.info.exports[i];
        
        if (exp1.name != exp2.name || exp1.type != exp2.type ||
            exp1.isDefault != exp2.isDefault) {
            return false;
        }
    }
    
    return true;
}

bool CMODModule::compareSubmodules(std::shared_ptr<CMODModule> other) const {
    return compareSubmodules(*other);
}

bool CMODModule::compareSubmodules(const CMODModule& other) const {
    if (submodules.size() != other.submodules.size()) {
        return false;
    }
    
    for (size_t i = 0; i < submodules.size(); ++i) {
        if (!submodules[i] || !other.submodules[i]) {
            return false;
        }
        if (!submodules[i]->equals(other.submodules[i])) {
            return false;
        }
    }
    
    return true;
}

bool CMODModule::compareMetadata(std::shared_ptr<CMODModule> other) const {
    return compareMetadata(*other);
}

bool CMODModule::compareMetadata(const CMODModule& other) const {
    return info.metadata == other.info.metadata;
}

// 克隆辅助实现
void CMODModule::copyTo(std::shared_ptr<CMODModule> target) const {
    target->info = info;
    target->path = path;
    target->searchPaths = searchPaths;
    target->submodules = submodules;
    target->error = error;
    target->loaded = loaded;
    target->enabled = enabled;
}

void CMODModule::deepCopyTo(std::shared_ptr<CMODModule> target) const {
    copyTo(target);
    
    // 深度复制子模块
    target->submodules.clear();
    for (const auto& submodule : submodules) {
        if (submodule) {
            auto clonedSubmodule = submodule->deepClone();
            target->addSubmodule(clonedSubmodule);
        }
    }
}

// 生成辅助实现
std::string CMODModule::generateJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"name\": \"" << info.name << "\",\n";
    oss << "  \"version\": \"" << info.version << "\",\n";
    oss << "  \"description\": \"" << info.description << "\",\n";
    oss << "  \"author\": \"" << info.author << "\",\n";
    oss << "  \"license\": \"" << info.license << "\",\n";
    oss << "  \"homepage\": \"" << info.homepage << "\",\n";
    oss << "  \"type\": " << static_cast<int>(info.type) << ",\n";
    oss << "  \"status\": " << static_cast<int>(info.status) << ",\n";
    oss << "  \"mainFile\": \"" << info.mainFile << "\",\n";
    oss << "  \"entryPoint\": \"" << info.entryPoint << "\"\n";
    oss << "}";
    return oss.str();
}

std::string CMODModule::generateXML() const {
    std::ostringstream oss;
    oss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    oss << "<module>\n";
    oss << "  <name>" << info.name << "</name>\n";
    oss << "  <version>" << info.version << "</version>\n";
    oss << "  <description>" << info.description << "</description>\n";
    oss << "  <author>" << info.author << "</author>\n";
    oss << "  <license>" << info.license << "</license>\n";
    oss << "  <homepage>" << info.homepage << "</homepage>\n";
    oss << "  <type>" << static_cast<int>(info.type) << "</type>\n";
    oss << "  <status>" << static_cast<int>(info.status) << "</status>\n";
    oss << "  <mainFile>" << info.mainFile << "</mainFile>\n";
    oss << "  <entryPoint>" << info.entryPoint << "</entryPoint>\n";
    oss << "</module>";
    return oss.str();
}

std::string CMODModule::generateYAML() const {
    std::ostringstream oss;
    oss << "name: " << info.name << "\n";
    oss << "version: " << info.version << "\n";
    oss << "description: " << info.description << "\n";
    oss << "author: " << info.author << "\n";
    oss << "license: " << info.license << "\n";
    oss << "homepage: " << info.homepage << "\n";
    oss << "type: " << static_cast<int>(info.type) << "\n";
    oss << "status: " << static_cast<int>(info.status) << "\n";
    oss << "mainFile: " << info.mainFile << "\n";
    oss << "entryPoint: " << info.entryPoint << "\n";
    return oss.str();
}

// 格式化辅助实现
std::string CMODModule::formatModule() const {
    return toString();
}

std::string CMODModule::minifyModule() const {
    return toString();
}

std::string CMODModule::beautifyModule() const {
    return toString();
}

} // namespace CHTL