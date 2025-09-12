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
    
    // 解析模块信息 - 使用简单的字符串分割而不是正则表达式
    std::istringstream iss(info);
    std::string line;
    
    while (std::getline(iss, line)) {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            // 去除空格和引号
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t\""));
            value.erase(value.find_last_not_of(" \t\"") + 1);
            
            if (key == "name") moduleInfo.name = value;
            else if (key == "version") moduleInfo.version = value;
            else if (key == "description") moduleInfo.description = value;
            else if (key == "author") moduleInfo.author = value;
            else if (key == "license") moduleInfo.license = value;
            else if (key == "category") moduleInfo.category = value;
            else if (key == "chtl_version") moduleInfo.minCHTLVersion = value;
        }
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

void CMODSystem::clear() {
    modules.clear();
}

} // namespace CHTL