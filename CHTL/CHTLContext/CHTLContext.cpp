#include "CHTLContext.h"

namespace CHTL {

CHTLContext::CHTLContext() {}

// --- [Template] Methods ---

void CHTLContext::addStyleTemplate(const StyleTemplate& tpl) {
    m_styleTemplates[tpl.name] = tpl;
}

const StyleTemplate* CHTLContext::getStyleTemplate(const std::string& name) const {
    auto it = m_styleTemplates.find(name);
    if (it != m_styleTemplates.end()) {
        return &it->second;
    }
    return nullptr;
}

void CHTLContext::addElementTemplate(ElementTemplate&& tpl) {
    m_elementTemplates[tpl.name] = std::move(tpl);
}

const ElementTemplate* CHTLContext::getElementTemplate(const std::string& name) const {
    auto it = m_elementTemplates.find(name);
    if (it != m_elementTemplates.end()) {
        return &it->second;
    }
    return nullptr;
}

void CHTLContext::addVarTemplate(const VarTemplate& tpl) {
    m_varTemplates[tpl.name] = tpl;
}

const VarTemplate* CHTLContext::getVarTemplate(const std::string& name) const {
    auto it = m_varTemplates.find(name);
    if (it != m_varTemplates.end()) {
        return &it->second;
    }
    return nullptr;
}


// --- [Custom] Methods ---

void CHTLContext::addCustomStyleTemplate(const CustomStyleTemplate& tpl) {
    m_customStyleTemplates[tpl.name] = tpl;
}

const CustomStyleTemplate* CHTLContext::getCustomStyleTemplate(const std::string& name) const {
    auto it = m_customStyleTemplates.find(name);
    if (it != m_customStyleTemplates.end()) {
        return &it->second;
    }
    return nullptr;
}

void CHTLContext::addCustomElementTemplate(CustomElementTemplate&& tpl) {
    m_customElementTemplates[tpl.name] = std::move(tpl);
}

const CustomElementTemplate* CHTLContext::getCustomElementTemplate(const std::string& name) const {
    auto it = m_customElementTemplates.find(name);
    if (it != m_customElementTemplates.end()) {
        return &it->second;
    }
    return nullptr;
}

} // namespace CHTL
