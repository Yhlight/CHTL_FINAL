#pragma once

#include <string>

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
};

} // namespace CHTL