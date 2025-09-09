#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace CHTL {

/**
 * @brief CMOD加载器
 * 负责加载CMOD模块
 */
class CMODLoader {
public:
    CMODLoader();
    ~CMODLoader();
    
    // 模块加载
    bool loadModule(const std::string& modulePath);
    std::string getModuleInfo(const std::string& modulePath);
    std::vector<std::string> getModuleExports(const std::string& modulePath);

private:
    struct ModuleInfo {
        std::string name;
        std::string version;
        std::string description;
        std::string content;
    };
    
    std::unordered_map<std::string, ModuleInfo> m_loadedModules;
};

} // namespace CHTL