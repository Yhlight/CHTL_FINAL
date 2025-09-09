#include "CMODSystem/CMODLoader.h"

namespace CHTL {

CMODLoader::CMODLoader() {
}

CMODLoader::~CMODLoader() = default;

bool CMODLoader::loadModule(const std::string& modulePath) {
    // TODO: 实现CMOD模块加载
    return false;
}

std::string CMODLoader::getModuleInfo(const std::string& modulePath) {
    // TODO: 实现模块信息获取
    return "";
}

std::vector<std::string> CMODLoader::getModuleExports(const std::string& modulePath) {
    // TODO: 实现模块导出获取
    return {};
}

} // namespace CHTL