#include "CHTLContext.h"

namespace CHTL {

void CHTLContext::registerTemplate(std::unique_ptr<TemplateDefinitionNode> templateNode) {
    if (templateNode) {
        // Use the template's name as the key.
        // Note: This doesn't handle name collisions yet.
        m_templates[templateNode->getTemplateName()] = std::move(templateNode);
    }
}

const TemplateDefinitionNode* CHTLContext::getTemplate(const std::string& name) const {
    auto it = m_templates.find(name);
    if (it != m_templates.end()) {
        return it->second.get();
    }
    return nullptr; // Not found
}

} // namespace CHTL
