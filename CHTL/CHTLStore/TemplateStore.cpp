#include "TemplateStore.h"

std::string TemplateStore::getQualifiedName(const std::string& ns, const std::string& name) const {
    if (ns.empty()) {
        return name;
    }
    return ns + "." + name;
}

bool TemplateStore::add(const std::string& ns, std::shared_ptr<TemplateDefinitionNode> templateNode) {
    if (!templateNode) {
        return false;
    }
    std::string key = getQualifiedName(ns, templateNode->name);
    std::cout << "[DEBUG] TemplateStore::add: Storing template with key '" << key << "'" << std::endl;
    if (m_store.count(key)) {
        return false; // Or handle error for redefinition
    }
    m_store[key] = templateNode;
    return true;
}

std::shared_ptr<TemplateDefinitionNode> TemplateStore::get(const std::string& ns, const std::string& name) const {
    // 1. Try to find in the current namespace
    std::string qualifiedName = getQualifiedName(ns, name);
    std::cout << "[DEBUG] TemplateStore::get: Attempting to find template with key '" << qualifiedName << "'" << std::endl;
    auto it = m_store.find(qualifiedName);
    if (it != m_store.end()) {
        std::cout << "  - Found." << std::endl;
        return it->second;
    }

    // 2. If not found, try to find in the global namespace (if we weren't already there)
    if (!ns.empty()) {
        it = m_store.find(name);
        if (it != m_store.end()) {
            return it->second;
        }
    }
    return nullptr;
}
