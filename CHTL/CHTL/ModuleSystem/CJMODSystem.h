#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace CHTL {

// CJMOD模块信息结构
struct CJMODInfo {
    std::string name;           // 模块名称
    std::string version;        // 版本号
    std::string description;    // 描述
    std::string author;         // 作者
    std::string license;        // 许可证
    std::vector<std::string> dependencies; // 依赖项
    std::map<std::string, std::string> metadata; // 其他元数据
};

// CJMOD导出函数类型
enum class CJMODFunctionType {
    Utility,        // 工具函数
    EventHandler,   // 事件处理函数
    Animation,      // 动画函数
    Router,         // 路由函数
    VirtualObject,  // 虚对象函数
    Custom          // 自定义函数
};

// CJMOD导出函数结构
struct CJMODFunction {
    std::string name;
    std::string description;
    CJMODFunctionType type;
    std::vector<std::string> parameters; // 参数列表
    std::string returnType; // 返回类型
    std::function<void()> implementation; // C++实现
    bool isPublic; // 是否公开导出
};

// CJMOD模块类
class CJMODModule {
public:
    CJMODModule();
    ~CJMODModule() = default;
    
    // 模块信息管理
    void setInfo(const CJMODInfo& info);
    const CJMODInfo& getInfo() const { return info; }
    
    // 源码管理
    void setSource(const std::string& source);
    const std::string& getSource() const { return source; }
    
    // 导出函数管理
    void addFunction(const CJMODFunction& func);
    void removeFunction(const std::string& name);
    bool hasFunction(const std::string& name) const;
    CJMODFunction getFunction(const std::string& name) const;
    std::vector<CJMODFunction> getAllFunctions() const;
    std::vector<CJMODFunction> getPublicFunctions() const;
    std::vector<CJMODFunction> getFunctionsByType(CJMODFunctionType type) const;
    
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
    CJMODInfo info;
    std::string source;
    std::map<std::string, CJMODFunction> functions;
    std::vector<std::string> dependencies;
    std::vector<std::string> validationErrors;
    
    void validateModule();
};

// CJMOD系统管理器
class CJMODSystem {
public:
    CJMODSystem();
    ~CJMODSystem() = default;
    
    // 模块管理
    void registerModule(std::shared_ptr<CJMODModule> module);
    void unregisterModule(const std::string& name);
    bool hasModule(const std::string& name) const;
    std::shared_ptr<CJMODModule> getModule(const std::string& name) const;
    std::vector<std::string> getAllModuleNames() const;
    
    // 函数搜索
    std::vector<std::string> searchFunctions(const std::string& query) const;
    std::vector<std::string> findFunctionsByType(CJMODFunctionType type) const;
    
    // 依赖解析
    std::vector<std::string> resolveDependencies(const std::string& moduleName) const;
    bool checkDependencyConflicts(const std::string& moduleName) const;
    
    // 模块加载
    std::shared_ptr<CJMODModule> loadModule(const std::string& path);
    bool saveModule(std::shared_ptr<CJMODModule> module, const std::string& path);
    
    // 函数调用
    bool callFunction(const std::string& moduleName, const std::string& functionName, 
                     const std::vector<std::string>& args = {});
    
    // 模块信息
    std::map<std::string, CJMODInfo> getAllModuleInfo() const;

private:
    std::map<std::string, std::shared_ptr<CJMODModule>> modules;
    std::map<std::string, std::string> modulePaths; // 模块名 -> 路径
    
    std::vector<std::string> findModuleFiles(const std::string& directory) const;
    std::shared_ptr<CJMODModule> parseModuleFile(const std::string& path) const;
};

} // namespace CHTL