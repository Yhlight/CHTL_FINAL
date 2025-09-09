#pragma once
#include "CMODSystem.h"
#include "CJMODSystem.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {

// 混合模块类型
enum class HybridModuleType {
    CMOD_ONLY,      // 仅CMOD
    CJMOD_ONLY,     // 仅CJMOD
    HYBRID          // 混合模块
};

// 混合模块信息
struct HybridModuleInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    HybridModuleType type;
    std::vector<std::string> dependencies;
    std::map<std::string, std::string> metadata;
};

// 混合模块类
class HybridModule {
public:
    HybridModule();
    ~HybridModule() = default;
    
    // 模块信息管理
    void setInfo(const HybridModuleInfo& info);
    const HybridModuleInfo& getInfo() const { return info; }
    
    // CMOD部分管理
    void setCMODModule(std::shared_ptr<CMODModule> cmod);
    std::shared_ptr<CMODModule> getCMODModule() const { return cmodModule; }
    bool hasCMODPart() const { return cmodModule != nullptr; }
    
    // CJMOD部分管理
    void setCJMODModule(std::shared_ptr<CJMODModule> cjmod);
    std::shared_ptr<CJMODModule> getCJMODModule() const { return cjmodModule; }
    bool hasCJMODPart() const { return cjmodModule != nullptr; }
    
    // 依赖管理
    void addDependency(const std::string& moduleName, const std::string& version = "");
    void removeDependency(const std::string& moduleName);
    bool hasDependency(const std::string& moduleName) const;
    std::vector<std::string> getDependencies() const;
    
    // 验证
    bool isValid() const;
    std::vector<std::string> getValidationErrors() const;
    
    // 序列化
    std::string toJSON() const;
    std::string toString() const;

private:
    HybridModuleInfo info;
    std::shared_ptr<CMODModule> cmodModule;
    std::shared_ptr<CJMODModule> cjmodModule;
    std::vector<std::string> dependencies;
    std::vector<std::string> validationErrors;
    
    void validateModule();
    void updateModuleType();
};

// 混合模块系统管理器
class HybridModuleSystem {
public:
    HybridModuleSystem();
    ~HybridModuleSystem() = default;
    
    // 模块管理
    void registerModule(std::shared_ptr<HybridModule> module);
    void unregisterModule(const std::string& name);
    bool hasModule(const std::string& name) const;
    std::shared_ptr<HybridModule> getModule(const std::string& name) const;
    std::vector<std::string> getAllModuleNames() const;
    
    // 模块搜索
    std::vector<std::string> searchModules(const std::string& query) const;
    std::vector<std::string> findModulesByType(HybridModuleType type) const;
    
    // 依赖解析
    std::vector<std::string> resolveDependencies(const std::string& moduleName) const;
    bool checkDependencyConflicts(const std::string& moduleName) const;
    
    // 模块加载
    std::shared_ptr<HybridModule> loadModule(const std::string& path);
    bool saveModule(std::shared_ptr<HybridModule> module, const std::string& path);
    
    // 函数调用（通过CJMOD部分）
    bool callFunction(const std::string& moduleName, const std::string& functionName, 
                     const std::vector<std::string>& args = {});
    
    // 导出项获取（通过CMOD部分）
    std::vector<ExportItem> getExports(const std::string& moduleName) const;
    
    // 模块信息
    std::map<std::string, HybridModuleInfo> getAllModuleInfo() const;
    
    // 子系统访问
    CMODSystem& getCMODSystem() { return cmodSystem; }
    CJMODSystem& getCJMODSystem() { return cjmodSystem; }

private:
    std::map<std::string, std::shared_ptr<HybridModule>> modules;
    std::map<std::string, std::string> modulePaths;
    
    CMODSystem cmodSystem;
    CJMODSystem cjmodSystem;
    
    std::vector<std::string> findModuleFiles(const std::string& directory) const;
    std::shared_ptr<HybridModule> parseModuleFile(const std::string& path) const;
    HybridModuleType detectModuleType(const std::string& content) const;
};

} // namespace CHTL