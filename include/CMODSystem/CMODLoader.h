#pragma once

#include <string>
#include <vector>

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
};

} // namespace CHTL