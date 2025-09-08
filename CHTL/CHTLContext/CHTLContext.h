#ifndef CHTL_CONTEXT_H
#define CHTL_CONTEXT_H

#include "TemplateData.h"
#include <string>
#include <unordered_map>

namespace CHTL {

class CHTLContext {
public:
    CHTLContext();

    // --- [Template] Methods ---
    void addStyleTemplate(const StyleTemplate& tpl);
    const StyleTemplate* getStyleTemplate(const std::string& name) const;
    void addElementTemplate(ElementTemplate&& tpl);
    const ElementTemplate* getElementTemplate(const std::string& name) const;
    void addVarTemplate(const VarTemplate& tpl);
    const VarTemplate* getVarTemplate(const std::string& name) const;

    // --- [Custom] Methods ---
    void addCustomStyleTemplate(const CustomStyleTemplate& tpl);
    const CustomStyleTemplate* getCustomStyleTemplate(const std::string& name) const;
    void addCustomElementTemplate(CustomElementTemplate&& tpl); // Added
    const CustomElementTemplate* getCustomElementTemplate(const std::string& name) const; // Added

private:
    // Stores for [Template] definitions
    std::unordered_map<std::string, StyleTemplate> m_styleTemplates;
    std::unordered_map<std::string, ElementTemplate> m_elementTemplates;
    std::unordered_map<std::string, VarTemplate> m_varTemplates;

    // Stores for [Custom] definitions
    std::unordered_map<std::string, CustomStyleTemplate> m_customStyleTemplates;
    std::unordered_map<std::string, CustomElementTemplate> m_customElementTemplates; // Added
};

} // namespace CHTL

#endif // CHTL_CONTEXT_H
