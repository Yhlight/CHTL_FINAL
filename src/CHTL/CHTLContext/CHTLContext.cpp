#include "CHTLContext.h"
#include <stdexcept>

CHTLContext::CHTLContext() {
    // Start with a default global namespace
    namespaceStack.push("global");
}

// --- Namespace Management ---
void CHTLContext::pushNamespace(const std::string& name) {
    namespaceStack.push(name);
}

void CHTLContext::popNamespace() {
    if (namespaceStack.size() > 1) { // Cannot pop the global namespace
        namespaceStack.pop();
    }
}

std::string CHTLContext::getCurrentNamespace() const {
    return namespaceStack.top();
}

// --- Template Management (now namespace-aware) ---
void CHTLContext::addStyleTemplate(const std::string& name, const StyleProperties& properties) {
    namespacedStyleTemplates[getCurrentNamespace()][name] = properties;
}

const CHTLContext::StyleProperties& CHTLContext::getStyleTemplate(const std::string& name, const std::string& ns) const {
    std::string targetNs = ns.empty() ? getCurrentNamespace() : ns;
    auto nsIt = namespacedStyleTemplates.find(targetNs);
    if (nsIt == namespacedStyleTemplates.end()) {
        throw std::runtime_error("Namespace not found: " + targetNs);
    }
    auto it = nsIt->second.find(name);
    if (it == nsIt->second.end()) {
        throw std::runtime_error("Style template '" + name + "' not found in namespace '" + targetNs + "'.");
    }
    return it->second;
}

void CHTLContext::addElementTemplate(const std::string& name, ElementTemplate elementTemplate) {
    namespacedElementTemplates[getCurrentNamespace()][name] = std::move(elementTemplate);
}

const CHTLContext::ElementTemplate& CHTLContext::getElementTemplate(const std::string& name, const std::string& ns) const {
    std::string targetNs = ns.empty() ? getCurrentNamespace() : ns;
    auto nsIt = namespacedElementTemplates.find(targetNs);
    if (nsIt == namespacedElementTemplates.end()) {
        throw std::runtime_error("Namespace not found: " + targetNs);
    }
    auto it = nsIt->second.find(name);
    if (it == nsIt->second.end()) {
        throw std::runtime_error("Element template '" + name + "' not found in namespace '" + targetNs + "'.");
    }
    return it->second;
}

void CHTLContext::addVarTemplate(const std::string& groupName, const VarGroup& vars) {
    namespacedVarTemplates[getCurrentNamespace()][groupName] = vars;
}

const std::string& CHTLContext::getVarTemplateValue(const std::string& groupName, const std::string& varName, const std::string& ns) const {
    std::string targetNs = ns.empty() ? getCurrentNamespace() : ns;
    auto nsIt = namespacedVarTemplates.find(targetNs);
    if (nsIt == namespacedVarTemplates.end()) {
        throw std::runtime_error("Namespace not found: " + targetNs);
    }
    auto groupIt = nsIt->second.find(groupName);
    if (groupIt == nsIt->second.end()) {
        throw std::runtime_error("Variable template group '" + groupName + "' not found in namespace '" + targetNs + "'.");
    }
    auto varIt = groupIt->second.find(varName);
    if (varIt == groupIt->second.end()) {
        throw std::runtime_error("Variable '" + varName + "' not found in group '" + groupName + "'.");
    }
    return varIt->second;
}


// --- Global CSS ---
void CHTLContext::addGlobalCSS(const std::string& rule) {
    globalCSS += rule + "\n";
}

const std::string& CHTLContext::getGlobalCSS() const {
    return globalCSS;
}
