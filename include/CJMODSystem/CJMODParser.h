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
    
    // 模块信息
    std::string getModuleName() const;
    std::string getModuleVersion() const;
    std::string getModuleDescription() const;
    std::string getModuleContent() const;
    std::string getCppAPI() const;

private:
    std::string m_moduleName;
    std::string m_moduleVersion;
    std::string m_moduleDescription;
    std::string m_moduleContent;
    std::string m_cppAPI;
    
    // 解析方法
    bool parseModuleInfo(const std::string& content);
    bool parseModuleContent(const std::string& content);
    bool parseCppAPI(const std::string& content);
};

} // namespace CHTL