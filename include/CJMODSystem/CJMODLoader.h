#pragma once

#include <string>
#include <unordered_map>

namespace CHTL {

/**
 * @brief CJMOD加载器
 * 负责加载CJMOD模块
 */
class CJMODLoader {
public:
    CJMODLoader();
    ~CJMODLoader();
    
    // 模块加载
    bool loadModule(const std::string& modulePath);
    std::string getModuleInfo(const std::string& modulePath);

private:
    struct CJMODInfo {
        std::string name;
        std::string version;
        std::string description;
        std::string content;
        std::string cppAPI;
    };
    
    std::unordered_map<std::string, CJMODInfo> m_loadedModules;
};

} // namespace CHTL