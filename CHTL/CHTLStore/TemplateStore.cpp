#include "TemplateStore.h"

bool TemplateStore::add(std::shared_ptr<TemplateDefinitionNode> templateNode) {
    if (!templateNode) {
        return false;
    }
    // Check if a template with this name already exists
    if (m_store.count(templateNode->name)) {
        return false; // Or handle error for redefinition
    }
    m_store[templateNode->name] = templateNode;
    return true;
}

std::shared_ptr<TemplateDefinitionNode> TemplateStore::get(const std::string& name) const {
    auto it = m_store.find(name);
    if (it != m_store.end()) {
        return it->second;
    }
    return nullptr;
}
