#ifndef CHTL_CMOD_SYSTEM_H
#define CHTL_CMOD_SYSTEM_H

#include <string>
#include <vector>
#include <memory>
#include "CHTLNode/BaseNode.h"

namespace CHTL {

struct CMODInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::string dependencies;
    std::string category;
    std::string minCHTLVersion;
    std::string maxCHTLVersion;
};

class CMODSystem {
public:
    CMODSystem();
    ~CMODSystem() = default;
    
    // 模块管理
    bool loadModule(const std::string& modulePath);
    bool unloadModule(const std::string& moduleName);
    bool isModuleLoaded(const std::string& moduleName) const;
    
    // 模块信息
    CMODInfo getModuleInfo(const std::string& moduleName) const;
    std::vector<std::string> getLoadedModules() const;
    
    // 模块内容
    NodePtr getModuleContent(const std::string& moduleName) const;
    
    // 错误处理
    void setErrorHandler(std::function<void(const std::string&)> handler);
    
private:
    std::unordered_map<std::string, NodePtr> loadedModules_;
    std::unordered_map<std::string, CMODInfo> moduleInfos_;
    std::function<void(const std::string&)> errorHandler_;
    
    void reportError(const std::string& message);
    bool parseModuleInfo(const std::string& content, CMODInfo& info);
};

} // namespace CHTL

#endif // CHTL_CMOD_SYSTEM_H