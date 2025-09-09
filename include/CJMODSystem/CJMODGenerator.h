#pragma once

#include <string>

namespace CHTL {

/**
 * @brief CJMOD生成器
 * 负责生成CJMOD模块
 */
class CJMODGenerator {
public:
    CJMODGenerator();
    ~CJMODGenerator();
    
    // 模块生成
    bool generateModule(const std::string& outputPath);

private:
};

} // namespace CHTL