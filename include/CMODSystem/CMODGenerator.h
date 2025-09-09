#pragma once

#include <string>
#include <vector>

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
    std::string m_moduleName;
    std::string m_moduleVersion;
    std::string m_moduleDescription;
    std::string m_moduleAuthor;
    std::string m_moduleLicense;
    std::string m_moduleContent;
    std::vector<std::string> m_moduleExports;
};

} // namespace CHTL