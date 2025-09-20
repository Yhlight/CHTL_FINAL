#include "CHTLContext.h"

namespace CHTL {

CHTLContext::CHTLContext() : current_namespace("") {}

void CHTLContext::addTemplateDefinition(const TemplateDefinitionNode& def) {
    namespaces[current_namespace].template_definitions[def.name] = def;
}

const TemplateDefinitionNode* CHTLContext::getTemplateDefinition(const std::string& name) {
    // 1. Look in the current namespace
    auto ns_it = namespaces.find(current_namespace);
    if (ns_it != namespaces.end()) {
        auto& ns_data = ns_it->second;
        auto def_it = ns_data.template_definitions.find(name);
        if (def_it != ns_data.template_definitions.end()) {
            return &def_it->second;
        }
    }

    // 2. Look in the default (global) namespace if we are not already in it
    if (!current_namespace.empty()) {
        ns_it = namespaces.find("");
        if (ns_it != namespaces.end()) {
            auto& ns_data = ns_it->second;
            auto def_it = ns_data.template_definitions.find(name);
            if (def_it != ns_data.template_definitions.end()) {
                return &def_it->second;
            }
        }
    }

    return nullptr;
}

} // namespace CHTL
