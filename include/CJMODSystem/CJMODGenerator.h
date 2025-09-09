#pragma once

#include <string>
#include <vector>

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
    std::string m_moduleName;
    std::string m_moduleVersion;
    std::string m_moduleDescription;
    std::string m_moduleAuthor;
    std::string m_moduleLicense;
    std::string m_moduleContent;
    std::string m_cppAPI;
    std::vector<std::string> m_moduleExports;
};

} // namespace CHTL