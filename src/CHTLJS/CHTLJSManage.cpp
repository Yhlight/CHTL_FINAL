#include "CHTLJS/CHTLJSManage.h"

namespace CHTL {

CHTLJSManage::CHTLJSManage() {
}

CHTLJSManage::~CHTLJSManage() = default;

void CHTLJSManage::addFunction(const std::string& name, const std::string& definition) {
    m_functions[name] = definition;
}

std::string CHTLJSManage::getFunction(const std::string& name) const {
    auto it = m_functions.find(name);
    return (it != m_functions.end()) ? it->second : "";
}

bool CHTLJSManage::hasFunction(const std::string& name) const {
    return m_functions.find(name) != m_functions.end();
}

} // namespace CHTL