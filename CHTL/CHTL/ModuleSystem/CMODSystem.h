#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "CHTLNode/BaseNode.h"

namespace CHTL {

// CMOD模块信息结构
struct CMODInfo {
    std::string name;           // 模块名称
    std::string version;        // 版本号
    std::string description;    // 描述
    std::string author;         // 作者
    std::string license;        // 许可证
    std::vector<std::string> dependencies; // 依赖项
    std::map<std::string, std::string> metadata; // 其他元数据
};

// 导出项类型
enum class ExportType {
    Template,   // 模板
    Custom,     // 自定义
    Style,      // 样式
    Element,    // 元素
    Var,        // 变量
    Namespace,  // 命名空间
    Configuration // 配置
};

// 导出项结构
struct ExportItem {
    ExportType type;
    std::string name;
    std::string description;
    std::shared_ptr<BaseNode> node; // 对应的AST节点
    bool isPublic; // 是否公开导出
};

// CMOD模块类
class CMODModule {
public:
    CMODModule();
    ~CMODModule() = default;
    
    // 模块信息管理
    void setInfo(const CMODInfo& info);
    const CMODInfo& getInfo() const { return info; }
    
    // 源码管理
    void setSource(const std::string& source);
    const std::string& getSource() const { return source; }
    
    // 导出表管理
    void addExport(const ExportItem& item);
    void removeExport(const std::string& name);
    bool hasExport(const std::string& name) const;
    ExportItem getExport(const std::string& name) const;
    std::vector<ExportItem> getAllExports() const;
    std::vector<ExportItem> getPublicExports() const;
    
    // 子模块管理
    void addSubmodule(const std::string& name, std::shared_ptr<CMODModule> submodule);
    void removeSubmodule(const std::string& name);
    bool hasSubmodule(const std::string& name) const;
    std::shared_ptr<CMODModule> getSubmodule(const std::string& name) const;
    std::vector<std::string> getSubmoduleNames() const;
    
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
    CMODInfo info;
    std::string source;
    std::map<std::string, ExportItem> exports;
    std::map<std::string, std::shared_ptr<CMODModule>> submodules;
    std::vector<std::string> dependencies;
    std::vector<std::string> validationErrors;
    
    void validateModule();
};

// CMOD系统管理器
class CMODSystem {
public:
    CMODSystem();
    ~CMODSystem() = default;
    
    // 模块管理
    void registerModule(std::shared_ptr<CMODModule> module);
    void unregisterModule(const std::string& name);
    bool hasModule(const std::string& name) const;
    std::shared_ptr<CMODModule> getModule(const std::string& name) const;
    std::vector<std::string> getAllModuleNames() const;
    
    // 模块搜索
    std::vector<std::string> searchModules(const std::string& query) const;
    std::vector<std::string> findModulesByType(ExportType type) const;
    
    // 依赖解析
    std::vector<std::string> resolveDependencies(const std::string& moduleName) const;
    bool checkDependencyConflicts(const std::string& moduleName) const;
    
    // 模块加载
    std::shared_ptr<CMODModule> loadModule(const std::string& path);
    bool saveModule(std::shared_ptr<CMODModule> module, const std::string& path);
    
    // 模块信息
    std::map<std::string, CMODInfo> getAllModuleInfo() const;

private:
    std::map<std::string, std::shared_ptr<CMODModule>> modules;
    std::map<std::string, std::string> modulePaths; // 模块名 -> 路径
    
    std::vector<std::string> findModuleFiles(const std::string& directory) const;
    std::shared_ptr<CMODModule> parseModuleFile(const std::string& path) const;
};

} // namespace CHTL