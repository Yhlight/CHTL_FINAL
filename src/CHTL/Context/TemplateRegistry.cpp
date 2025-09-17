#include "TemplateRegistry.h"

namespace CHTL {

void TemplateRegistry::registerTemplate(const std::string& name, std::shared_ptr<TemplateNode> node) {
    m_templates[name] = node;
}

std::shared_ptr<TemplateNode> TemplateRegistry::lookupTemplate(const std::string& name) {
    if (m_templates.count(name)) {
        return m_templates[name];
    }
    return nullptr; // Or throw an error
}

} // namespace CHTL
