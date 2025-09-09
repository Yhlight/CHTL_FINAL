#include "CHTLJS/CHTLJSContext.h"

namespace CHTL {

CHTLJSContext::CHTLJSContext() {
}

CHTLJSContext::~CHTLJSContext() = default;

void CHTLJSContext::addFunction(const std::string& name, const std::string& definition) {
    m_functions[name] = definition;
}

std::string CHTLJSContext::getFunction(const std::string& name) const {
    auto it = m_functions.find(name);
    return (it != m_functions.end()) ? it->second : "";
}

bool CHTLJSContext::hasFunction(const std::string& name) const {
    return m_functions.find(name) != m_functions.end();
}

} // namespace CHTL