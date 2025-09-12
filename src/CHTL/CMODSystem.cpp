#include "CMODSystem.h"
#include <regex>
#include <fstream>
#include <sstream>

namespace CHTL {

CMODSystem::CMODSystem() {
    // 初始化模块系统
}

CMODInfo CMODSystem::parseModuleInfo(const std::string& info) {
    CMODInfo moduleInfo;
    
    // 解析模块信息
    std::regex infoRegex(R"((\w+)\s*=\s*"([^"]*)")");
    std::sregex_iterator iter(info.begin(), info.end(), infoRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string key = iter->str(1);
        std::string value = iter->str(2);
        
        if (key == "name") moduleInfo.name = value;
        else if (key == "version") moduleInfo.version = value;
        else if (key == "description") moduleInfo.description = value;
        else if (key == "author") moduleInfo.author = value;
        else if (key == "license") moduleInfo.license = value;
        else if (key == "category") moduleInfo.category = value;
        else if (key == "chtl_version") moduleInfo.minCHTLVersion = value;
    }
    
    return moduleInfo;
}

void CMODSystem::registerModule(std::unique_ptr<CMODModule> module) {
    modules[module->info.name] = std::move(module);
}

CMODModule* CMODSystem::getModule(const std::string& name) {
    auto it = modules.find(name);
    return (it != modules.end()) ? it->second.get() : nullptr;
}

const std::map<std::string, std::unique_ptr<CMODModule>>& CMODSystem::getAllModules() const {
    return modules;
}

std::string CMODSystem::processImport(const std::string& moduleName) {
    auto module = getModule(moduleName);
    if (module) {
        return "// Imported module: " + moduleName;
    }
    return "";
}

} // namespace CHTL