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
    std::string getModuleContent() const;

private:
    std::string m_moduleName;
    std::string m_moduleVersion;
    std::string m_moduleDescription;
    std::string m_moduleContent;
    
    // 解析方法
    bool parseModuleInfo(const std::string& content);
    bool parseModuleContent(const std::string& content);
};

} // namespace CHTL