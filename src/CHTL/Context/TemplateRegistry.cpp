#include "TemplateRegistry.h"

namespace CHTL {

void TemplateRegistry::registerDefinition(const std::string& name, std::shared_ptr<BaseNode> node) {
    m_templates[name] = node;
}

std::shared_ptr<BaseNode> TemplateRegistry::lookupDefinition(const std::string& name) {
    if (m_templates.count(name)) {
        return m_templates[name];
    }
    return nullptr;
}

void TemplateRegistry::clear() {
    m_templates.clear();
}

} // namespace CHTL
