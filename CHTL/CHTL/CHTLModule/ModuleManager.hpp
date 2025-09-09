#pragma once

#include <CHTL/CHTLNode/BaseNode.hpp>
#include <string>
#include <memory>
#include <map>
#include <vector>

namespace CHTL {

/**
 * @brief 模块类型枚举
 */
enum class ModuleType {
    CMOD,   // CHTL 模块
    CJMOD   // CHTL JS 模块
};

/**
 * @brief 模块信息结构
 */
struct ModuleInfo {
    std::string name;
    ModuleType type;
    std::string path;
    std::shared_ptr<BaseNode> ast;
    std::map<std::string, std::string> exports;
    std::vector<std::string> dependencies;
    bool isLoaded;
    
    ModuleInfo() : type(ModuleType::CMOD), isLoaded(false) {}
};

/**
 * @brief 模块管理器
 * 
 * 根据 CHTL.md 文档实现
 * 管理 CMOD 和 CJMOD 模块的创建、导入和管理
 */
class ModuleManager {
public:
    static ModuleManager& getInstance();
    
    // 模块注册和管理
    bool registerModule(const std::string& name, ModuleType type, const std::string& path);
    bool loadModule(const std::string& name);
    bool unloadModule(const std::string& name);
    
    // 模块查询
    std::shared_ptr<ModuleInfo> getModule(const std::string& name);
    std::vector<std::string> getModuleNames() const;
    std::vector<std::string> getModulesByType(ModuleType type) const;
    
    // 模块依赖管理
    bool addDependency(const std::string& moduleName, const std::string& dependency);
    std::vector<std::string> getDependencies(const std::string& moduleName) const;
    std::vector<std::string> getDependents(const std::string& moduleName) const;
    
    // 模块导出管理
    bool addExport(const std::string& moduleName, const std::string& exportName, const std::string& value);
    std::string getExport(const std::string& moduleName, const std::string& exportName) const;
    std::map<std::string, std::string> getExports(const std::string& moduleName) const;
    
    // 模块解析
    std::string resolveModuleReference(const std::string& reference) const;
    std::string resolveExportReference(const std::string& moduleName, const std::string& exportName) const;
    
    // 模块验证
    bool validateModule(const std::string& name) const;
    bool validateDependencies(const std::string& name) const;
    
    // 模块清理
    void clearModules();
    void clearModule(const std::string& name);
    
private:
    ModuleManager() = default;
    ~ModuleManager() = default;
    ModuleManager(const ModuleManager&) = delete;
    ModuleManager& operator=(const ModuleManager&) = delete;
    
    std::map<std::string, std::shared_ptr<ModuleInfo>> modules_;
    std::map<std::string, std::vector<std::string>> dependencyGraph_;
    
    // 内部方法
    bool loadModuleFromFile(const std::string& name, const std::string& path);
    bool parseModuleAST(const std::string& name, const std::string& content);
    void buildDependencyGraph();
    bool hasCircularDependency(const std::string& moduleName) const;
};

} // namespace CHTL