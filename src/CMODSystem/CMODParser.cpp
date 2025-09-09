#include "CMODSystem/CMODParser.h"
#include <fstream>
#include <sstream>

namespace CHTL {

CMODParser::CMODParser() {
}

CMODParser::~CMODParser() = default;

bool CMODParser::parseModule(const std::string& modulePath) {
    // 解析CMOD模块文件
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
    
    return true;
}

std::string CMODParser::getModuleName() const {
    return m_moduleName;
}

std::string CMODParser::getModuleVersion() const {
    return m_moduleVersion;
}

bool CMODParser::parseModuleInfo(const std::string& content) {
    // 解析模块信息部分
    size_t infoStart = content.find("[ModuleInfo]");
    if (infoStart == std::string::npos) {
        return false;
    }
    
    size_t infoEnd = content.find("[/ModuleInfo]", infoStart);
    if (infoEnd == std::string::npos) {
        return false;
    }
    
    std::string infoContent = content.substr(infoStart + 12, infoEnd - infoStart - 12);
    
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
    
    // 解析模块版本
    size_t versionStart = infoContent.find("version:");
    if (versionStart != std::string::npos) {
        size_t versionEnd = infoContent.find("\n", versionStart);
        if (versionEnd != std::string::npos) {
            m_moduleVersion = infoContent.substr(versionStart + 8, versionEnd - versionStart - 8);
            // 去除空白字符
            m_moduleVersion.erase(0, m_moduleVersion.find_first_not_of(" \t"));
            m_moduleVersion.erase(m_moduleVersion.find_last_not_of(" \t") + 1);
        }
    }
    
    return true;
}

bool CMODParser::parseModuleContent(const std::string& content) {
    // 解析模块内容部分
    size_t contentStart = content.find("[ModuleContent]");
    if (contentStart == std::string::npos) {
        return false;
    }
    
    size_t contentEnd = content.find("[/ModuleContent]", contentStart);
    if (contentEnd == std::string::npos) {
        return false;
    }
    
    m_moduleContent = content.substr(contentStart + 15, contentEnd - contentStart - 15);
    
    return true;
}

std::string CMODParser::getModuleDescription() const {
    return m_moduleDescription;
}

std::string CMODParser::getModuleContent() const {
    return m_moduleContent;
}

} // namespace CHTL