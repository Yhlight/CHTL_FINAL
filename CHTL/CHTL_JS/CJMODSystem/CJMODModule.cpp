#include "CJMODModule.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <regex>

namespace CHTL_JS {

// CJMODModule 实现
CJMODModule::CJMODModule() 
    : loaded(false), enabled(true) {
    info.type = CJMODModuleType::UNKNOWN;
    info.status = CJMODModuleStatus::UNLOADED;
}

CJMODModule::CJMODModule(const std::string& name, const std::string& version) 
    : loaded(false), enabled(true) {
    info.name = name;
    info.version = version;
    info.type = CJMODModuleType::UNKNOWN;
    info.status = CJMODModuleStatus::UNLOADED;
}

CJMODModule::CJMODModule(const CJMODModuleInfo& info) 
    : info(info), loaded(false), enabled(true) {
}

CJMODModule::~CJMODModule() = default;

// 基本信息实现
void CJMODModule::setName(const std::string& name) {
    info.name = name;
}

std::string CJMODModule::getName() const {
    return info.name;
}

void CJMODModule::setVersion(const std::string& version) {
    info.version = version;
}

std::string CJMODModule::getVersion() const {
    return info.version;
}

void CJMODModule::setDescription(const std::string& description) {
    info.description = description;
}

std::string CJMODModule::getDescription() const {
    return info.description;
}

void CJMODModule::setAuthor(const std::string& author) {
    info.author = author;
}

std::string CJMODModule::getAuthor() const {
    return info.author;
}

void CJMODModule::setLicense(const std::string& license) {
    info.license = license;
}

std::string CJMODModule::getLicense() const {
    return info.license;
}

void CJMODModule::setHomepage(const std::string& homepage) {
    info.homepage = homepage;
}

std::string CJMODModule::getHomepage() const {
    return info.homepage;
}

// 模块信息实现
void CJMODModule::setInfo(const CJMODModuleInfo& info) {
    this->info = info;
}

CJMODModuleInfo CJMODModule::getInfo() const {
    return info;
}

void CJMODModule::setType(CJMODModuleType type) {
    info.type = type;
}

CJMODModuleType CJMODModule::getType() const {
    return info.type;
}

void CJMODModule::setStatus(CJMODModuleStatus status) {
    info.status = status;
}

CJMODModuleStatus CJMODModule::getStatus() const {
    return info.status;
}

void CJMODModule::setMainFile(const std::string& mainFile) {
    info.mainFile = mainFile;
}

std::string CJMODModule::getMainFile() const {
    return info.mainFile;
}

void CJMODModule::setEntryPoint(const std::string& entryPoint) {
    info.entryPoint = entryPoint;
}

std::string CJMODModule::getEntryPoint() const {
    return info.entryPoint;
}

// 关键词管理实现
void CJMODModule::addKeyword(const std::string& keyword) {
    if (std::find(info.keywords.begin(), info.keywords.end(), keyword) == info.keywords.end()) {
        info.keywords.push_back(keyword);
    }
}

void CJMODModule::removeKeyword(const std::string& keyword) {
    info.keywords.erase(std::remove(info.keywords.begin(), info.keywords.end(), keyword), info.keywords.end());
}

void CJMODModule::clearKeywords() {
    info.keywords.clear();
}

std::vector<std::string> CJMODModule::getKeywords() const {
    return info.keywords;
}

bool CJMODModule::hasKeyword(const std::string& keyword) const {
    return std::find(info.keywords.begin(), info.keywords.end(), keyword) != info.keywords.end();
}

// 依赖管理实现
void CJMODModule::addDependency(const CJMODModuleDependency& dependency) {
    // 检查是否已存在同名依赖
    auto it = std::find_if(info.dependencies.begin(), info.dependencies.end(),
        [&dependency](const CJMODModuleDependency& dep) {
            return dep.name == dependency.name;
        });
    
    if (it != info.dependencies.end()) {
        *it = dependency; // 更新现有依赖
    } else {
        info.dependencies.push_back(dependency); // 添加新依赖
    }
}

void CJMODModule::addDependency(const std::string& name, const std::string& version, bool required) {
    CJMODModuleDependency dependency(name, version, required);
    addDependency(dependency);
}

void CJMODModule::removeDependency(const std::string& name) {
    info.dependencies.erase(std::remove_if(info.dependencies.begin(), info.dependencies.end(),
        [&name](const CJMODModuleDependency& dep) {
            return dep.name == name;
        }), info.dependencies.end());
}

void CJMODModule::clearDependencies() {
    info.dependencies.clear();
}

std::vector<CJMODModuleDependency> CJMODModule::getDependencies() const {
    return info.dependencies;
}

std::vector<CJMODModuleDependency> CJMODModule::getRequiredDependencies() const {
    std::vector<CJMODModuleDependency> required;
    std::copy_if(info.dependencies.begin(), info.dependencies.end(), std::back_inserter(required),
        [](const CJMODModuleDependency& dep) {
            return dep.required;
        });
    return required;
}

std::vector<CJMODModuleDependency> CJMODModule::getOptionalDependencies() const {
    std::vector<CJMODModuleDependency> optional;
    std::copy_if(info.dependencies.begin(), info.dependencies.end(), std::back_inserter(optional),
        [](const CJMODModuleDependency& dep) {
            return dep.optional;
        });
    return optional;
}

bool CJMODModule::hasDependency(const std::string& name) const {
    return std::any_of(info.dependencies.begin(), info.dependencies.end(),
        [&name](const CJMODModuleDependency& dep) {
            return dep.name == name;
        });
}

CJMODModuleDependency CJMODModule::getDependency(const std::string& name) const {
    auto it = std::find_if(info.dependencies.begin(), info.dependencies.end(),
        [&name](const CJMODModuleDependency& dep) {
            return dep.name == name;
        });
    
    if (it != info.dependencies.end()) {
        return *it;
    }
    
    return CJMODModuleDependency(); // 返回空依赖
}

// 导出管理实现
void CJMODModule::addExport(const CJMODModuleExport& export_) {
    // 检查是否已存在同名导出
    auto it = std::find_if(info.exports.begin(), info.exports.end(),
        [&export_](const CJMODModuleExport& exp) {
            return exp.name == export_.name;
        });
    
    if (it != info.exports.end()) {
        *it = export_; // 更新现有导出
    } else {
        info.exports.push_back(export_); // 添加新导出
    }
}

void CJMODModule::addExport(const std::string& name, const std::any& value, const std::string& type) {
    CJMODModuleExport export_(name, value, type);
    addExport(export_);
}

void CJMODModule::addDefaultExport(const std::any& value, const std::string& type) {
    CJMODModuleExport export_("default", value, type);
    export_.isDefault = true;
    addExport(export_);
}

void CJMODModule::addFunctionExport(const std::string& name, const std::any& value, const std::string& type) {
    CJMODModuleExport export_(name, value, type);
    export_.isFunction = true;
    addExport(export_);
}

void CJMODModule::addClassExport(const std::string& name, const std::any& value, const std::string& type) {
    CJMODModuleExport export_(name, value, type);
    export_.isClass = true;
    addExport(export_);
}

void CJMODModule::addVariableExport(const std::string& name, const std::any& value, const std::string& type) {
    CJMODModuleExport export_(name, value, type);
    export_.isVariable = true;
    addExport(export_);
}

void CJMODModule::removeExport(const std::string& name) {
    info.exports.erase(std::remove_if(info.exports.begin(), info.exports.end(),
        [&name](const CJMODModuleExport& exp) {
            return exp.name == name;
        }), info.exports.end());
}

void CJMODModule::clearExports() {
    info.exports.clear();
}

std::vector<CJMODModuleExport> CJMODModule::getExports() const {
    return info.exports;
}

std::vector<CJMODModuleExport> CJMODModule::getNamedExports() const {
    std::vector<CJMODModuleExport> named;
    std::copy_if(info.exports.begin(), info.exports.end(), std::back_inserter(named),
        [](const CJMODModuleExport& exp) {
            return !exp.isDefault;
        });
    return named;
}

std::vector<CJMODModuleExport> CJMODModule::getFunctionExports() const {
    std::vector<CJMODModuleExport> functions;
    std::copy_if(info.exports.begin(), info.exports.end(), std::back_inserter(functions),
        [](const CJMODModuleExport& exp) {
            return exp.isFunction;
        });
    return functions;
}

std::vector<CJMODModuleExport> CJMODModule::getClassExports() const {
    std::vector<CJMODModuleExport> classes;
    std::copy_if(info.exports.begin(), info.exports.end(), std::back_inserter(classes),
        [](const CJMODModuleExport& exp) {
            return exp.isClass;
        });
    return classes;
}

std::vector<CJMODModuleExport> CJMODModule::getVariableExports() const {
    std::vector<CJMODModuleExport> variables;
    std::copy_if(info.exports.begin(), info.exports.end(), std::back_inserter(variables),
        [](const CJMODModuleExport& exp) {
            return exp.isVariable;
        });
    return variables;
}

CJMODModuleExport CJMODModule::getDefaultExport() const {
    auto it = std::find_if(info.exports.begin(), info.exports.end(),
        [](const CJMODModuleExport& exp) {
            return exp.isDefault;
        });
    
    if (it != info.exports.end()) {
        return *it;
    }
    
    return CJMODModuleExport(); // 返回空导出
}

bool CJMODModule::hasExport(const std::string& name) const {
    return std::any_of(info.exports.begin(), info.exports.end(),
        [&name](const CJMODModuleExport& exp) {
            return exp.name == name;
        });
}

CJMODModuleExport CJMODModule::getExport(const std::string& name) const {
    auto it = std::find_if(info.exports.begin(), info.exports.end(),
        [&name](const CJMODModuleExport& exp) {
            return exp.name == name;
        });
    
    if (it != info.exports.end()) {
        return *it;
    }
    
    return CJMODModuleExport(); // 返回空导出
}

std::any CJMODModule::getExportValue(const std::string& name) const {
    CJMODModuleExport export_ = getExport(name);
    return export_.value;
}

// 元数据管理实现
void CJMODModule::setMetadata(const std::map<std::string, std::any>& metadata) {
    info.metadata = metadata;
}

std::map<std::string, std::any> CJMODModule::getMetadata() const {
    return info.metadata;
}

void CJMODModule::addMetadata(const std::string& key, const std::any& value) {
    info.metadata[key] = value;
}

std::any CJMODModule::getMetadata(const std::string& key) const {
    auto it = info.metadata.find(key);
    return it != info.metadata.end() ? it->second : std::any{};
}

bool CJMODModule::hasMetadata(const std::string& key) const {
    return info.metadata.find(key) != info.metadata.end();
}

void CJMODModule::removeMetadata(const std::string& key) {
    info.metadata.erase(key);
}

void CJMODModule::clearMetadata() {
    info.metadata.clear();
}

// 子模块管理实现
void CJMODModule::addSubmodule(std::shared_ptr<CJMODModule> submodule) {
    if (submodule) {
        submodules.push_back(submodule);
    }
}

void CJMODModule::removeSubmodule(const std::string& name) {
    submodules.erase(std::remove_if(submodules.begin(), submodules.end(),
        [&name](const std::shared_ptr<CJMODModule>& submodule) {
            return submodule && submodule->getName() == name;
        }), submodules.end());
}

void CJMODModule::clearSubmodules() {
    submodules.clear();
}

std::vector<std::shared_ptr<CJMODModule>> CJMODModule::getSubmodules() const {
    return submodules;
}

std::shared_ptr<CJMODModule> CJMODModule::getSubmodule(const std::string& name) const {
    auto it = std::find_if(submodules.begin(), submodules.end(),
        [&name](const std::shared_ptr<CJMODModule>& submodule) {
            return submodule && submodule->getName() == name;
        });
    
    return it != submodules.end() ? *it : nullptr;
}

bool CJMODModule::hasSubmodule(const std::string& name) const {
    return getSubmodule(name) != nullptr;
}

// 路径管理实现
void CJMODModule::setPath(const std::string& path) {
    this->path = path;
}

std::string CJMODModule::getPath() const {
    return path;
}

void CJMODModule::addSearchPath(const std::string& path) {
    if (std::find(searchPaths.begin(), searchPaths.end(), path) == searchPaths.end()) {
        searchPaths.push_back(path);
    }
}

void CJMODModule::removeSearchPath(const std::string& path) {
    searchPaths.erase(std::remove(searchPaths.begin(), searchPaths.end(), path), searchPaths.end());
}

void CJMODModule::clearSearchPaths() {
    searchPaths.clear();
}

std::vector<std::string> CJMODModule::getSearchPaths() const {
    return searchPaths;
}

// 加载状态实现
void CJMODModule::setLoaded(bool loaded) {
    this->loaded = loaded;
    info.status = loaded ? CJMODModuleStatus::LOADED : CJMODModuleStatus::UNLOADED;
}

bool CJMODModule::isLoaded() const {
    return loaded;
}

void CJMODModule::setEnabled(bool enabled) {
    this->enabled = enabled;
}

bool CJMODModule::isEnabled() const {
    return enabled;
}

void CJMODModule::setError(const std::string& error) {
    this->error = error;
    info.status = CJMODModuleStatus::ERROR;
}

std::string CJMODModule::getError() const {
    return error;
}

bool CJMODModule::hasError() const {
    return !error.empty();
}

void CJMODModule::clearError() {
    error.clear();
    if (info.status == CJMODModuleStatus::ERROR) {
        info.status = CJMODModuleStatus::UNLOADED;
    }
}

// 验证实现
bool CJMODModule::isValid() const {
    return validateModule() && validateInfo() && validateDependencies() && 
           validateExports() && validateSubmodules() && validateMetadata();
}

bool CJMODModule::isComplete() const {
    return !info.name.empty() && !info.version.empty() && info.type != CJMODModuleType::UNKNOWN;
}

std::vector<std::string> CJMODModule::validate() const {
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
bool CJMODModule::equals(std::shared_ptr<CJMODModule> other) const {
    return other && compareModule(other);
}

bool CJMODModule::equals(const CJMODModule& other) const {
    return compareModule(other);
}

// 克隆实现
std::shared_ptr<CJMODModule> CJMODModule::clone() const {
    auto cloned = std::make_shared<CJMODModule>(info);
    copyTo(cloned);
    return cloned;
}

std::shared_ptr<CJMODModule> CJMODModule::deepClone() const {
    auto cloned = std::make_shared<CJMODModule>(info);
    deepCopyTo(cloned);
    return cloned;
}

// 转换实现
std::string CJMODModule::toJSON() const {
    return generateJSON();
}

std::string CJMODModule::toXML() const {
    return generateXML();
}

std::string CJMODModule::toYAML() const {
    return generateYAML();
}

std::string CJMODModule::toString() const {
    return info.name + "@" + info.version;
}

std::string CJMODModule::toDebugString() const {
    std::ostringstream oss;
    oss << "CJMODModule{name='" << info.name
        << "', version='" << info.version
        << "', type=" << static_cast<int>(info.type)
        << ", status=" << static_cast<int>(info.status)
        << ", loaded=" << loaded
        << ", enabled=" << enabled << "}";
    return oss.str();
}

// 格式化实现
std::string CJMODModule::format() const {
    return formatModule();
}

std::string CJMODModule::minify() const {
    return minifyModule();
}

std::string CJMODModule::beautify() const {
    return beautifyModule();
}

// 统计实现
size_t CJMODModule::getDependencyCount() const {
    return info.dependencies.size();
}

size_t CJMODModule::getExportCount() const {
    return info.exports.size();
}

size_t CJMODModule::getFunctionExportCount() const {
    return getFunctionExports().size();
}

size_t CJMODModule::getClassExportCount() const {
    return getClassExports().size();
}

size_t CJMODModule::getVariableExportCount() const {
    return getVariableExports().size();
}

size_t CJMODModule::getSubmoduleCount() const {
    return submodules.size();
}

size_t CJMODModule::getMetadataCount() const {
    return info.metadata.size();
}

// 重置实现
void CJMODModule::reset() {
    info = CJMODModuleInfo();
    path.clear();
    searchPaths.clear();
    submodules.clear();
    error.clear();
    loaded = false;
    enabled = true;
}

// 验证辅助实现
bool CJMODModule::validateModule() const {
    return !info.name.empty() && !info.version.empty();
}

bool CJMODModule::validateInfo() const {
    return info.type != CJMODModuleType::UNKNOWN;
}

bool CJMODModule::validateDependencies() const {
    // 简化的依赖验证
    return true;
}

bool CJMODModule::validateExports() const {
    // 简化的导出验证
    return true;
}

bool CJMODModule::validateSubmodules() const {
    for (const auto& submodule : submodules) {
        if (!submodule || !submodule->isValid()) {
            return false;
        }
    }
    return true;
}

bool CJMODModule::validateMetadata() const {
    // 简化的元数据验证
    return true;
}

// 比较辅助实现
bool CJMODModule::compareModule(std::shared_ptr<CJMODModule> other) const {
    return compareModule(*other);
}

bool CJMODModule::compareModule(const CJMODModule& other) const {
    return compareInfo(other) && compareDependencies(other) && 
           compareExports(other) && compareSubmodules(other) && compareMetadata(other);
}

bool CJMODModule::compareInfo(std::shared_ptr<CJMODModule> other) const {
    return compareInfo(*other);
}

bool CJMODModule::compareInfo(const CJMODModule& other) const {
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

bool CJMODModule::compareDependencies(std::shared_ptr<CJMODModule> other) const {
    return compareDependencies(*other);
}

bool CJMODModule::compareDependencies(const CJMODModule& other) const {
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

bool CJMODModule::compareExports(std::shared_ptr<CJMODModule> other) const {
    return compareExports(*other);
}

bool CJMODModule::compareExports(const CJMODModule& other) const {
    if (info.exports.size() != other.info.exports.size()) {
        return false;
    }
    
    for (size_t i = 0; i < info.exports.size(); ++i) {
        const auto& exp1 = info.exports[i];
        const auto& exp2 = other.info.exports[i];
        
        if (exp1.name != exp2.name || exp1.type != exp2.type ||
            exp1.isDefault != exp2.isDefault || exp1.isFunction != exp2.isFunction ||
            exp1.isClass != exp2.isClass || exp1.isVariable != exp2.isVariable) {
            return false;
        }
    }
    
    return true;
}

bool CJMODModule::compareSubmodules(std::shared_ptr<CJMODModule> other) const {
    return compareSubmodules(*other);
}

bool CJMODModule::compareSubmodules(const CJMODModule& other) const {
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

bool CJMODModule::compareMetadata(std::shared_ptr<CJMODModule> other) const {
    return compareMetadata(*other);
}

bool CJMODModule::compareMetadata(const CJMODModule& other) const {
    if (info.metadata.size() != other.info.metadata.size()) {
        return false;
    }
    for (const auto& pair : info.metadata) {
        auto it = other.info.metadata.find(pair.first);
        if (it == other.info.metadata.end()) {
            return false;
        }
        // 由于std::any不能直接比较，我们只比较键
        // 在实际应用中，可能需要更复杂的比较逻辑
    }
    return true;
}

// 克隆辅助实现
void CJMODModule::copyTo(std::shared_ptr<CJMODModule> target) const {
    target->info = info;
    target->path = path;
    target->searchPaths = searchPaths;
    target->submodules = submodules;
    target->error = error;
    target->loaded = loaded;
    target->enabled = enabled;
}

void CJMODModule::deepCopyTo(std::shared_ptr<CJMODModule> target) const {
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
std::string CJMODModule::generateJSON() const {
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

std::string CJMODModule::generateXML() const {
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

std::string CJMODModule::generateYAML() const {
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
std::string CJMODModule::formatModule() const {
    return toString();
}

std::string CJMODModule::minifyModule() const {
    return toString();
}

std::string CJMODModule::beautifyModule() const {
    return toString();
}

} // namespace CHTL_JS