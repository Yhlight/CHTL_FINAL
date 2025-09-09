#include "ModuleManager.hpp"
#include <CHTL/CHTLLexer/Lexer.hpp>
#include <CHTL/CHTLParser/Parser.hpp>
#include <CHTL/CHTLJS/CHTLJSLexer/CHTLJSLexer.hpp>
#include <CHTL/CHTLJS/CHTLJSParser/CHTLJSParser.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <set>
#include <functional>

namespace CHTL {

ModuleManager& ModuleManager::getInstance() {
    static ModuleManager instance;
    return instance;
}

bool ModuleManager::registerModule(const std::string& name, ModuleType type, const std::string& path) {
    if (modules_.find(name) != modules_.end()) {
        return false; // 模块已存在
    }
    
    auto moduleInfo = std::make_shared<ModuleInfo>();
    moduleInfo->name = name;
    moduleInfo->type = type;
    moduleInfo->path = path;
    moduleInfo->isLoaded = false;
    
    modules_[name] = moduleInfo;
    return true;
}

bool ModuleManager::loadModule(const std::string& name) {
    auto it = modules_.find(name);
    if (it == modules_.end()) {
        return false; // 模块不存在
    }
    
    auto moduleInfo = it->second;
    if (moduleInfo->isLoaded) {
        return true; // 模块已加载
    }
    
    // 加载模块文件
    if (!loadModuleFromFile(name, moduleInfo->path)) {
        return false;
    }
    
    moduleInfo->isLoaded = true;
    return true;
}

bool ModuleManager::unloadModule(const std::string& name) {
    auto it = modules_.find(name);
    if (it == modules_.end()) {
        return false; // 模块不存在
    }
    
    auto moduleInfo = it->second;
    moduleInfo->isLoaded = false;
    moduleInfo->ast.reset();
    moduleInfo->exports.clear();
    
    return true;
}

std::shared_ptr<ModuleInfo> ModuleManager::getModule(const std::string& name) {
    auto it = modules_.find(name);
    if (it == modules_.end()) {
        return nullptr;
    }
    return it->second;
}

std::vector<std::string> ModuleManager::getModuleNames() const {
    std::vector<std::string> names;
    for (const auto& [name, module] : modules_) {
        names.push_back(name);
    }
    return names;
}

std::vector<std::string> ModuleManager::getModulesByType(ModuleType type) const {
    std::vector<std::string> names;
    for (const auto& [name, module] : modules_) {
        if (module->type == type) {
            names.push_back(name);
        }
    }
    return names;
}

bool ModuleManager::addDependency(const std::string& moduleName, const std::string& dependency) {
    auto it = modules_.find(moduleName);
    if (it == modules_.end()) {
        return false; // 模块不存在
    }
    
    auto moduleInfo = it->second;
    moduleInfo->dependencies.push_back(dependency);
    
    // 更新依赖图
    dependencyGraph_[moduleName].push_back(dependency);
    
    return true;
}

std::vector<std::string> ModuleManager::getDependencies(const std::string& moduleName) const {
    auto it = modules_.find(moduleName);
    if (it == modules_.end()) {
        return {};
    }
    return it->second->dependencies;
}

std::vector<std::string> ModuleManager::getDependents(const std::string& moduleName) const {
    std::vector<std::string> dependents;
    for (const auto& [name, deps] : dependencyGraph_) {
        if (std::find(deps.begin(), deps.end(), moduleName) != deps.end()) {
            dependents.push_back(name);
        }
    }
    return dependents;
}

bool ModuleManager::addExport(const std::string& moduleName, const std::string& exportName, const std::string& value) {
    auto it = modules_.find(moduleName);
    if (it == modules_.end()) {
        return false; // 模块不存在
    }
    
    auto moduleInfo = it->second;
    moduleInfo->exports[exportName] = value;
    
    return true;
}

std::string ModuleManager::getExport(const std::string& moduleName, const std::string& exportName) const {
    auto it = modules_.find(moduleName);
    if (it == modules_.end()) {
        return "";
    }
    
    auto moduleInfo = it->second;
    auto exportIt = moduleInfo->exports.find(exportName);
    if (exportIt == moduleInfo->exports.end()) {
        return "";
    }
    
    return exportIt->second;
}

std::map<std::string, std::string> ModuleManager::getExports(const std::string& moduleName) const {
    auto it = modules_.find(moduleName);
    if (it == modules_.end()) {
        return {};
    }
    return it->second->exports;
}

std::string ModuleManager::resolveModuleReference(const std::string& reference) const {
    // 解析模块引用，格式：@ModuleName
    if (reference.empty() || reference[0] != '@') {
        return reference;
    }
    
    std::string moduleName = reference.substr(1);
    auto module = const_cast<ModuleManager*>(this)->getModule(moduleName);
    if (!module || !module->isLoaded) {
        return reference;
    }
    
    return module->path;
}

std::string ModuleManager::resolveExportReference(const std::string& moduleName, const std::string& exportName) const {
    return getExport(moduleName, exportName);
}

bool ModuleManager::validateModule(const std::string& name) const {
    auto it = modules_.find(name);
    if (it == modules_.end()) {
        return false; // 模块不存在
    }
    
    auto moduleInfo = it->second;
    if (!moduleInfo->isLoaded) {
        return false; // 模块未加载
    }
    
    if (!moduleInfo->ast) {
        return false; // 模块 AST 无效
    }
    
    return true;
}

bool ModuleManager::validateDependencies(const std::string& name) const {
    auto it = modules_.find(name);
    if (it == modules_.end()) {
        return false; // 模块不存在
    }
    
    auto moduleInfo = it->second;
    for (const auto& dependency : moduleInfo->dependencies) {
        if (!validateModule(dependency)) {
            return false; // 依赖模块无效
        }
    }
    
    return true;
}

void ModuleManager::clearModules() {
    modules_.clear();
    dependencyGraph_.clear();
}

void ModuleManager::clearModule(const std::string& name) {
    auto it = modules_.find(name);
    if (it != modules_.end()) {
        modules_.erase(it);
    }
    
    auto depIt = dependencyGraph_.find(name);
    if (depIt != dependencyGraph_.end()) {
        dependencyGraph_.erase(depIt);
    }
}

bool ModuleManager::loadModuleFromFile(const std::string& name, const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();
    
    return parseModuleAST(name, content);
}

bool ModuleManager::parseModuleAST(const std::string& name, const std::string& content) {
    auto it = modules_.find(name);
    if (it == modules_.end()) {
        return false;
    }
    
    auto moduleInfo = it->second;
    
    try {
        if (moduleInfo->type == ModuleType::CMOD) {
            // 解析 CHTL 模块
            Lexer lexer(content);
            Parser parser(lexer);
            moduleInfo->ast = parser.parse();
            
            if (parser.hasError()) {
                return false;
            }
        } else if (moduleInfo->type == ModuleType::CJMOD) {
            // 解析 CHTL JS 模块
            std::istringstream stream(content);
            CHTLJSLexer jsLexer(stream);
            CHTLJSParser jsParser(stream);
            // 注意：这里需要适配 CHTLJSParser 的接口
            // moduleInfo->ast = jsParser.parse();
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

void ModuleManager::buildDependencyGraph() {
    dependencyGraph_.clear();
    for (const auto& [name, module] : modules_) {
        dependencyGraph_[name] = module->dependencies;
    }
}

bool ModuleManager::hasCircularDependency(const std::string& moduleName) const {
    // 简单的循环依赖检测
    std::set<std::string> visited;
    std::set<std::string> recursionStack;
    
    std::function<bool(const std::string&)> hasCycle = [&](const std::string& name) -> bool {
        if (recursionStack.find(name) != recursionStack.end()) {
            return true; // 发现循环
        }
        
        if (visited.find(name) != visited.end()) {
            return false; // 已访问过，无循环
        }
        
        visited.insert(name);
        recursionStack.insert(name);
        
        auto it = dependencyGraph_.find(name);
        if (it != dependencyGraph_.end()) {
            for (const auto& dep : it->second) {
                if (hasCycle(dep)) {
                    return true;
                }
            }
        }
        
        recursionStack.erase(name);
        return false;
    };
    
    return hasCycle(moduleName);
}

} // namespace CHTL