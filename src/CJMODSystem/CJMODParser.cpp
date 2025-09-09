#include "CJMODSystem/CJMODParser.h"
#include <fstream>
#include <sstream>

namespace CHTL {

CJMODParser::CJMODParser() {
}

CJMODParser::~CJMODParser() = default;

bool CJMODParser::parseModule(const std::string& modulePath) {
    // 解析CJMOD模块文件
    std::ifstream file(modulePath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    // 解析模块信息
    if (!parseModuleInfo(content)) {
        return false;
    }
    
    // 解析模块内容
    if (!parseModuleContent(content)) {
        return false;
    }
    
    // 解析C++ API
    if (!parseCppAPI(content)) {
        return false;
    }
    
    return true;
}

bool CJMODParser::parseModuleInfo(const std::string& content) {
    // 解析模块信息部分
    size_t infoStart = content.find("[CJMODInfo]");
    if (infoStart == std::string::npos) {
        return false;
    }
    
    size_t infoEnd = content.find("[/CJMODInfo]", infoStart);
    if (infoEnd == std::string::npos) {
        return false;
    }
    
    std::string infoContent = content.substr(infoStart + 11, infoEnd - infoStart - 11);
    
    // 解析模块名称
    size_t nameStart = infoContent.find("name:");
    if (nameStart != std::string::npos) {
        size_t nameEnd = infoContent.find("\n", nameStart);
        if (nameEnd != std::string::npos) {
            m_moduleName = infoContent.substr(nameStart + 5, nameEnd - nameStart - 5);
            // 去除空白字符
            m_moduleName.erase(0, m_moduleName.find_first_not_of(" \t"));
            m_moduleName.erase(m_moduleName.find_last_not_of(" \t") + 1);
        }
    }
    
    return true;
}

bool CJMODParser::parseModuleContent(const std::string& content) {
    // 解析模块内容部分
    size_t contentStart = content.find("[CJMODContent]");
    if (contentStart == std::string::npos) {
        return false;
    }
    
    size_t contentEnd = content.find("[/CJMODContent]", contentStart);
    if (contentEnd == std::string::npos) {
        return false;
    }
    
    m_moduleContent = content.substr(contentStart + 14, contentEnd - contentStart - 14);
    
    return true;
}

bool CJMODParser::parseCppAPI(const std::string& content) {
    // 解析C++ API部分
    size_t apiStart = content.find("[CppAPI]");
    if (apiStart == std::string::npos) {
        return false;
    }
    
    size_t apiEnd = content.find("[/CppAPI]", apiStart);
    if (apiEnd == std::string::npos) {
        return false;
    }
    
    m_cppAPI = content.substr(apiStart + 8, apiEnd - apiStart - 8);
    
    return true;
}

} // namespace CHTL