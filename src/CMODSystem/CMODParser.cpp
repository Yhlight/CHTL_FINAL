#include "CMODSystem/CMODParser.h"

namespace CHTL {

CMODParser::CMODParser() {
}

CMODParser::~CMODParser() = default;

bool CMODParser::parseModule(const std::string& modulePath) {
    // TODO: 实现CMOD模块解析
    return false;
}

std::string CMODParser::getModuleName() const {
    return m_moduleName;
}

std::string CMODParser::getModuleVersion() const {
    return m_moduleVersion;
}

std::string CMODParser::getModuleDescription() const {
    return m_moduleDescription;
}

} // namespace CHTL