#ifndef CHTL_JS_CJMOD_SYSTEM_H
#define CHTL_JS_CJMOD_SYSTEM_H

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL_JS {

class CJMODSystem {
private:
    std::string modulePath;
    std::vector<std::string> loadedModules;
    std::map<std::string, std::string> moduleCache;
    std::vector<std::string> errors;
    bool debugMode;

public:
    CJMODSystem();
    CJMODSystem(const std::string& path);
    virtual ~CJMODSystem() = default;
    
    // 基本属性
    const std::string& getModulePath() const { return modulePath; }
    void setModulePath(const std::string& path) { modulePath = path; }
    
    // 模块管理
    bool loadModule(const std::string& moduleName);
    bool unloadModule(const std::string& moduleName);
    bool isModuleLoaded(const std::string& moduleName) const;
    std::vector<std::string> getLoadedModules() const { return loadedModules; }
    
    // 模块缓存
    void cacheModule(const std::string& moduleName, const std::string& content);
    std::string getCachedModule(const std::string& moduleName) const;
    bool isModuleCached(const std::string& moduleName) const;
    void clearCache();
    
    // 错误管理
    const std::vector<std::string>& getErrors() const { return errors; }
    bool hasErrors() const { return !errors.empty(); }
    void clearErrors();
    
    // 模式设置
    bool isDebugMode() const { return debugMode; }
    void setDebugMode(bool mode) { debugMode = mode; }
    
    // 工具方法
    std::string resolveModulePath(const std::string& moduleName) const;
    bool moduleExists(const std::string& moduleName) const;
    std::string getModuleContent(const std::string& moduleName) const;
    
    // 字符串表示
    std::string toString() const;
    std::string toDebugString() const;
    
    // 序列化
    std::string toJSON() const;
    std::string toXML() const;
    std::string toYAML() const;
    
    // 克隆
    CJMODSystem clone() const;
    
    // 比较
    bool operator==(const CJMODSystem& other) const;
    bool operator!=(const CJMODSystem& other) const;
};

} // namespace CHTL_JS

#endif // CHTL_JS_CJMOD_SYSTEM_H