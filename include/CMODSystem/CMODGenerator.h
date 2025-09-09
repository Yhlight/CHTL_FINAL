#pragma once

#include <string>

namespace CHTL {

/**
 * @brief CMOD生成器
 * 负责生成CMOD模块
 */
class CMODGenerator {
public:
    CMODGenerator();
    ~CMODGenerator();
    
    // 模块生成
    bool generateModule(const std::string& outputPath);
    std::string getGeneratedModule() const;

private:
    std::string m_generatedModule;
};

} // namespace CHTL