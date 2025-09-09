#include "CJMODSystem.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace CHTL {

// CJMODModule 实现
CJMODModule::CJMODModule() {
    info.name = "";
    info.version = "1.0.0";
    info.description = "";
    info.author = "";
    info.license = "MIT";
}

void CJMODModule::setInfo(const CJMODInfo& moduleInfo) {
    info = moduleInfo;
    validateModule();
}

void CJMODModule::setSource(const std::string& moduleSource) {
    source = moduleSource;
    validateModule();
}

void CJMODModule::addFunction(const CJMODFunction& func) {
    functions[func.name] = func;
    std::cout << "Added function: " << func.name << " (type: " << static_cast<int>(func.type) << ")" << std::endl;
}

void CJMODModule::removeFunction(const std::string& name) {
    auto it = functions.find(name);
    if (it != functions.end()) {
        functions.erase(it);
        std::cout << "Removed function: " << name << std::endl;
    }
}

bool CJMODModule::hasFunction(const std::string& name) const {
    return functions.find(name) != functions.end();
}

CJMODFunction CJMODModule::getFunction(const std::string& name) const {
    auto it = functions.find(name);
    if (it != functions.end()) {
        return it->second;
    }
    
    // 返回空函数
    CJMODFunction emptyFunc;
    emptyFunc.name = "";
    emptyFunc.description = "";
    emptyFunc.type = CJMODFunctionType::Utility;
    emptyFunc.returnType = "void";
    emptyFunc.isPublic = false;
    return emptyFunc;
}

std::vector<CJMODFunction> CJMODModule::getAllFunctions() const {
    std::vector<CJMODFunction> result;
    for (const auto& pair : functions) {
        result.push_back(pair.second);
    }
    return result;
}

std::vector<CJMODFunction> CJMODModule::getPublicFunctions() const {
    std::vector<CJMODFunction> result;
    for (const auto& pair : functions) {
        if (pair.second.isPublic) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::vector<CJMODFunction> CJMODModule::getFunctionsByType(CJMODFunctionType type) const {
    std::vector<CJMODFunction> result;
    for (const auto& pair : functions) {
        if (pair.second.type == type) {
            result.push_back(pair.second);
        }
    }
    return result;
}

void CJMODModule::addDependency(const std::string& moduleName, const std::string& version) {
    std::string dep = version.empty() ? moduleName : moduleName + "@" + version;
    if (std::find(dependencies.begin(), dependencies.end(), dep) == dependencies.end()) {
        dependencies.push_back(dep);
        std::cout << "Added dependency: " << dep << std::endl;
    }
}

void CJMODModule::removeDependency(const std::string& moduleName) {
    dependencies.erase(std::remove_if(dependencies.begin(), dependencies.end(),
        [&moduleName](const std::string& dep) {
            return dep.find(moduleName) == 0;
        }), dependencies.end());
    std::cout << "Removed dependency: " << moduleName << std::endl;
}

bool CJMODModule::hasDependency(const std::string& moduleName) const {
    return std::find_if(dependencies.begin(), dependencies.end(),
        [&moduleName](const std::string& dep) {
            return dep.find(moduleName) == 0;
        }) != dependencies.end();
}

std::vector<std::string> CJMODModule::getDependencies() const {
    return dependencies;
}

bool CJMODModule::isValid() const {
    return validationErrors.empty();
}

std::vector<std::string> CJMODModule::getValidationErrors() const {
    return validationErrors;
}

void CJMODModule::validateModule() {
    validationErrors.clear();
    
    if (info.name.empty()) {
        validationErrors.push_back("Module name is required");
    }
    
    if (info.version.empty()) {
        validationErrors.push_back("Module version is required");
    }
    
    if (source.empty()) {
        validationErrors.push_back("Module source is required");
    }
    
    // 检查函数是否有重复名称
    std::set<std::string> functionNames;
    for (const auto& pair : functions) {
        if (functionNames.count(pair.first)) {
            validationErrors.push_back("Duplicate function name: " + pair.first);
        }
        functionNames.insert(pair.first);
    }
}

std::string CJMODModule::toJSON() const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"name\": \"" << info.name << "\",\n";
    ss << "  \"version\": \"" << info.version << "\",\n";
    ss << "  \"description\": \"" << info.description << "\",\n";
    ss << "  \"author\": \"" << info.author << "\",\n";
    ss << "  \"license\": \"" << info.license << "\",\n";
    ss << "  \"dependencies\": [";
    for (size_t i = 0; i < dependencies.size(); ++i) {
        ss << "\"" << dependencies[i] << "\"";
        if (i < dependencies.size() - 1) ss << ", ";
    }
    ss << "],\n";
    ss << "  \"functions\": [";
    size_t functionCount = 0;
    for (const auto& pair : functions) {
        if (functionCount > 0) ss << ", ";
        ss << "{\n";
        ss << "    \"name\": \"" << pair.second.name << "\",\n";
        ss << "    \"type\": " << static_cast<int>(pair.second.type) << ",\n";
        ss << "    \"description\": \"" << pair.second.description << "\",\n";
        ss << "    \"returnType\": \"" << pair.second.returnType << "\",\n";
        ss << "    \"isPublic\": " << (pair.second.isPublic ? "true" : "false") << ",\n";
        ss << "    \"parameters\": [";
        for (size_t i = 0; i < pair.second.parameters.size(); ++i) {
            ss << "\"" << pair.second.parameters[i] << "\"";
            if (i < pair.second.parameters.size() - 1) ss << ", ";
        }
        ss << "]\n";
        ss << "  }";
        functionCount++;
    }
    ss << "]\n";
    ss << "}";
    return ss.str();
}

std::string CJMODModule::toString() const {
    std::stringstream ss;
    ss << "CJMOD Module: " << info.name << " v" << info.version << "\n";
    ss << "Description: " << info.description << "\n";
    ss << "Author: " << info.author << "\n";
    ss << "License: " << info.license << "\n";
    ss << "Dependencies: ";
    for (size_t i = 0; i < dependencies.size(); ++i) {
        ss << dependencies[i];
        if (i < dependencies.size() - 1) ss << ", ";
    }
    ss << "\n";
    ss << "Functions (" << functions.size() << "):\n";
    for (const auto& pair : functions) {
        ss << "  - " << pair.second.name << " (" << static_cast<int>(pair.second.type) << ")\n";
    }
    return ss.str();
}

// CJMODSystem 实现
CJMODSystem::CJMODSystem() {
    // 初始化系统
}

void CJMODSystem::registerModule(std::shared_ptr<CJMODModule> module) {
    if (module && !module->getInfo().name.empty()) {
        modules[module->getInfo().name] = module;
        std::cout << "Registered CJMOD module: " << module->getInfo().name << std::endl;
    }
}

void CJMODSystem::unregisterModule(const std::string& name) {
    auto it = modules.find(name);
    if (it != modules.end()) {
        modules.erase(it);
        modulePaths.erase(name);
        std::cout << "Unregistered CJMOD module: " << name << std::endl;
    }
}

bool CJMODSystem::hasModule(const std::string& name) const {
    return modules.find(name) != modules.end();
}

std::shared_ptr<CJMODModule> CJMODSystem::getModule(const std::string& name) const {
    auto it = modules.find(name);
    if (it != modules.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::string> CJMODSystem::getAllModuleNames() const {
    std::vector<std::string> result;
    for (const auto& pair : modules) {
        result.push_back(pair.first);
    }
    return result;
}

std::vector<std::string> CJMODSystem::searchFunctions(const std::string& query) const {
    std::vector<std::string> result;
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
    
    for (const auto& modulePair : modules) {
        auto functions = modulePair.second->getAllFunctions();
        for (const auto& func : functions) {
            std::string functionName = func.name;
            std::string lowerName = functionName;
            std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
            
            if (lowerName.find(lowerQuery) != std::string::npos) {
                result.push_back(modulePair.first + "::" + functionName);
            }
        }
    }
    return result;
}

std::vector<std::string> CJMODSystem::findFunctionsByType(CJMODFunctionType type) const {
    std::vector<std::string> result;
    for (const auto& modulePair : modules) {
        auto functions = modulePair.second->getFunctionsByType(type);
        for (const auto& func : functions) {
            result.push_back(modulePair.first + "::" + func.name);
        }
    }
    return result;
}

std::vector<std::string> CJMODSystem::resolveDependencies(const std::string& moduleName) const {
    std::vector<std::string> resolved;
    std::set<std::string> visited;
    
    std::function<void(const std::string&)> resolve = [&](const std::string& name) {
        if (visited.count(name)) return;
        visited.insert(name);
        
        auto module = getModule(name);
        if (module) {
            auto deps = module->getDependencies();
            for (const auto& dep : deps) {
                resolve(dep);
            }
            resolved.push_back(name);
        }
    };
    
    resolve(moduleName);
    return resolved;
}

bool CJMODSystem::checkDependencyConflicts(const std::string& moduleName) const {
    auto module = getModule(moduleName);
    if (!module) return false;
    
    auto deps = module->getDependencies();
    for (const auto& dep : deps) {
        if (!hasModule(dep)) {
            std::cerr << "Missing CJMOD dependency: " << dep << std::endl;
            return true;
        }
    }
    return false;
}

std::shared_ptr<CJMODModule> CJMODSystem::loadModule(const std::string& path) {
    try {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Cannot open CJMOD file: " << path << std::endl;
            return nullptr;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        file.close();
        
        // 这里应该解析CHTL JS文件并创建模块
        auto module = std::make_shared<CJMODModule>();
        module->setSource(content);
        
        // 从路径提取模块名
        std::filesystem::path filePath(path);
        std::string moduleName = filePath.stem().string();
        
        CJMODInfo info;
        info.name = moduleName;
        info.version = "1.0.0";
        info.description = "Loaded from " + path;
        module->setInfo(info);
        
        modulePaths[moduleName] = path;
        return module;
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading CJMOD: " << e.what() << std::endl;
        return nullptr;
    }
}

bool CJMODSystem::saveModule(std::shared_ptr<CJMODModule> module, const std::string& path) {
    try {
        std::ofstream file(path);
        if (!file.is_open()) {
            std::cerr << "Cannot create CJMOD file: " << path << std::endl;
            return false;
        }
        
        file << module->getSource();
        file.close();
        
        modulePaths[module->getInfo().name] = path;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error saving CJMOD: " << e.what() << std::endl;
        return false;
    }
}

bool CJMODSystem::callFunction(const std::string& moduleName, const std::string& functionName, 
                               const std::vector<std::string>& args) {
    auto module = getModule(moduleName);
    if (!module) {
        std::cerr << "Module not found: " << moduleName << std::endl;
        return false;
    }
    
    auto func = module->getFunction(functionName);
    if (func.name.empty()) {
        std::cerr << "Function not found: " << functionName << " in module " << moduleName << std::endl;
        return false;
    }
    
    try {
        if (func.implementation) {
            func.implementation();
            std::cout << "Called function: " << moduleName << "::" << functionName << std::endl;
            return true;
        } else {
            std::cerr << "Function implementation not available: " << functionName << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error calling function: " << e.what() << std::endl;
        return false;
    }
}

std::map<std::string, CJMODInfo> CJMODSystem::getAllModuleInfo() const {
    std::map<std::string, CJMODInfo> result;
    for (const auto& pair : modules) {
        result[pair.first] = pair.second->getInfo();
    }
    return result;
}

std::vector<std::string> CJMODSystem::findModuleFiles(const std::string& directory) const {
    std::vector<std::string> result;
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".cjjs") {
                result.push_back(entry.path().string());
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error scanning directory: " << e.what() << std::endl;
    }
    return result;
}

std::shared_ptr<CJMODModule> CJMODSystem::parseModuleFile(const std::string& path) const {
    // 这里应该使用CHTL JS解析器来解析模块文件
    return std::make_shared<CJMODModule>();
}

} // namespace CHTL