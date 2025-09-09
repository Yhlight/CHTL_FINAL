#include "CHTL/CHTLManage.h"

namespace CHTL {

CHTLManage::CHTLManage(std::shared_ptr<CHTLContext> context) 
    : m_context(context) {
}

CHTLManage::~CHTLManage() = default;

void CHTLManage::addModule(const std::string& name, std::shared_ptr<BaseNode> module) {
    m_modules[name] = module;
}

std::shared_ptr<BaseNode> CHTLManage::getModule(const std::string& name) const {
    auto it = m_modules.find(name);
    return (it != m_modules.end()) ? it->second : nullptr;
}

bool CHTLManage::hasModule(const std::string& name) const {
    return m_modules.find(name) != m_modules.end();
}

void CHTLManage::removeModule(const std::string& name) {
    m_modules.erase(name);
}

const std::unordered_map<std::string, std::shared_ptr<BaseNode>>& CHTLManage::getModules() const {
    return m_modules;
}

void CHTLManage::clearModules() {
    m_modules.clear();
}

} // namespace CHTL