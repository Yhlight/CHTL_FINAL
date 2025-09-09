#include "CHTLContext.h"
#include <stdexcept>

// --- Style Templates ---
void CHTLContext::addStyleTemplate(const std::string& name, const StyleProperties& properties) {
    styleTemplates[name] = properties;
}

const CHTLContext::StyleProperties& CHTLContext::getStyleTemplate(const std::string& name) const {
    auto it = styleTemplates.find(name);
    if (it == styleTemplates.end()) {
        throw std::runtime_error("Style template not found: " + name);
    }
    return it->second;
}

bool CHTLContext::hasStyleTemplate(const std::string& name) const {
    return styleTemplates.find(name) != styleTemplates.end();
}


// --- Element Templates ---
void CHTLContext::addElementTemplate(const std::string& name, ElementTemplate elementTemplate) {
    elementTemplates[name] = std::move(elementTemplate);
}

const CHTLContext::ElementTemplate& CHTLContext::getElementTemplate(const std::string& name) const {
    auto it = elementTemplates.find(name);
    if (it == elementTemplates.end()) {
        throw std::runtime_error("Element template not found: " + name);
    }
    return it->second;
}

bool CHTLContext::hasElementTemplate(const std::string& name) const {
    return elementTemplates.find(name) != elementTemplates.end();
}


// --- Variable Templates ---
void CHTLContext::addVarTemplate(const std::string& groupName, const VarGroup& vars) {
    varTemplates[groupName] = vars;
}

const std::string& CHTLContext::getVarTemplateValue(const std::string& groupName, const std::string& varName) const {
    auto groupIt = varTemplates.find(groupName);
    if (groupIt == varTemplates.end()) {
        throw std::runtime_error("Variable template group not found: " + groupName);
    }
    const auto& varGroup = groupIt->second;
    auto varIt = varGroup.find(varName);
    if (varIt == varGroup.end()) {
        throw std::runtime_error("Variable not found in group '" + groupName + "': " + varName);
    }
    return varIt->second;
}

bool CHTLContext::hasVarTemplate(const std::string& groupName) const {
    return varTemplates.find(groupName) != varTemplates.end();
}

// --- Global CSS ---
void CHTLContext::addGlobalCSS(const std::string& rule) {
    globalCSS += rule + "\n";
}

const std::string& CHTLContext::getGlobalCSS() const {
    return globalCSS;
}
