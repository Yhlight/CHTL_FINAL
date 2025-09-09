#pragma once

#include <string>

namespace CHTL {

/**
 * @brief CMOD解析器
 * 负责解析CMOD模块
 */
class CMODParser {
public:
    CMODParser();
    ~CMODParser();
    
    // 模块解析
    bool parseModule(const std::string& modulePath);
    
    // 模块信息
    std::string getModuleName() const;
    std::string getModuleVersion() const;
    std::string getModuleDescription() const;

private:
    std::string m_moduleName;
    std::string m_moduleVersion;
    std::string m_moduleDescription;
};

} // namespace CHTL