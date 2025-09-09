#pragma once

#include <string>

namespace CHTL {

/**
 * @brief CJMOD API
 * 提供CJMOD模块的API接口
 */
class CJMODAPI {
public:
    CJMODAPI();
    ~CJMODAPI();
    
    // API操作
    bool initialize();
    std::string processCode(const std::string& code);

private:
};

} // namespace CHTL