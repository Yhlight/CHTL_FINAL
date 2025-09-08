#include "CHTLContext.h"

namespace CHTL {

// --- Template Methods ---

void CHTLContext::registerTemplate(std::unique_ptr<TemplateDefinitionNode> templateNode) {
    if (templateNode) {
        m_templates[templateNode->getTemplateName()] = std::move(templateNode);
    }
}

const TemplateDefinitionNode* CHTLContext::getTemplate(const std::string& name) const {
    auto it = m_templates.find(name);
    if (it != m_templates.end()) {
        return it->second.get();
    }
    return nullptr;
}

// --- Custom Methods ---

void CHTLContext::registerCustom(std::unique_ptr<CustomDefinitionNode> customNode) {
    if (customNode) {
        m_customs[customNode->getCustomName()] = std::move(customNode);
    }
}

const CustomDefinitionNode* CHTLContext::getCustom(const std::string& name) const {
    auto it = m_customs.find(name);
    if (it != m_customs.end()) {
        return it->second.get();
    }
    return nullptr;
}


} // namespace CHTL
