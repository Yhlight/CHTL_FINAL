#include "CMODSystem/CMODLoader.h"
#include "CMODSystem/CMODParser.h"
#include <sstream>

namespace CHTL {

CMODLoader::CMODLoader() {
}

CMODLoader::~CMODLoader() = default;

bool CMODLoader::loadModule(const std::string& modulePath) {
    // 加载CMOD模块
    CMODParser parser;
    if (!parser.parseModule(modulePath)) {
        return false;
    }
    
    // 存储模块信息
    m_loadedModules[modulePath] = {
        parser.getModuleName(),
        parser.getModuleVersion(),
        parser.getModuleDescription(),
        parser.getModuleContent()
    };
    
    return true;
}

std::string CMODLoader::getModuleInfo(const std::string& modulePath) {
    auto it = m_loadedModules.find(modulePath);
    if (it == m_loadedModules.end()) {
        return "";
    }
    
    const auto& module = it->second;
    std::stringstream info;
    info << "Module: " << module.name << std::endl;
    info << "Version: " << module.version << std::endl;
    info << "Description: " << module.description << std::endl;
    
    return info.str();
}

std::vector<std::string> CMODLoader::getModuleExports(const std::string& modulePath) {
    // TODO: 实现模块导出获取
    return {};
}

} // namespace CHTL