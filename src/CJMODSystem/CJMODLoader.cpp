#include "CJMODSystem/CJMODLoader.h"
#include "CJMODSystem/CJMODParser.h"
#include <sstream>

namespace CHTL {

CJMODLoader::CJMODLoader() {
}

CJMODLoader::~CJMODLoader() = default;

bool CJMODLoader::loadModule(const std::string& modulePath) {
    // 加载CJMOD模块
    CJMODParser parser;
    if (!parser.parseModule(modulePath)) {
        return false;
    }
    
    // 存储模块信息
    m_loadedModules[modulePath] = {
        parser.getModuleName(),
        parser.getModuleVersion(),
        parser.getModuleDescription(),
        parser.getModuleContent(),
        parser.getCppAPI()
    };
    
    return true;
}

std::string CJMODLoader::getModuleInfo(const std::string& modulePath) {
    auto it = m_loadedModules.find(modulePath);
    if (it == m_loadedModules.end()) {
        return "";
    }
    
    const auto& module = it->second;
    std::stringstream info;
    info << "CJMOD Module: " << module.name << std::endl;
    info << "Version: " << module.version << std::endl;
    info << "Description: " << module.description << std::endl;
    info << "C++ API: " << module.cppAPI << std::endl;
    
    return info.str();
}

} // namespace CHTL