#pragma once

#include <string>

namespace CHTL {

/**
 * @brief CJMOD解析器
 * 负责解析CJMOD模块
 */
class CJMODParser {
public:
    CJMODParser();
    ~CJMODParser();
    
    // 模块解析
    bool parseModule(const std::string& modulePath);

private:
};

} // namespace CHTL