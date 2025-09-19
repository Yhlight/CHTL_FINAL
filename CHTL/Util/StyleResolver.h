#ifndef STYLE_RESOLVER_H
#define STYLE_RESOLVER_H

#include "../CHTLNode/TemplateDefinitionNode.h"
#include "../CHTLNode/AttributeNode.h"
#include "../CHTLNode/StyleNode.h"
#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <stdexcept>

namespace CHTL {
namespace StyleResolver {

inline void resolveStyleTemplate(
    const std::string& template_name,
    const std::map<std::string, std::map<std::string, TemplateDefinitionNode>>& all_templates,
    std::map<std::string, AttributeNode>& properties,
    std::unordered_set<std::string>& visited_templates)
{
    if (visited_templates.count(template_name)) {
        return; // Avoid infinite recursion
    }
    visited_templates.insert(template_name);

    const TemplateDefinitionNode* def = nullptr;
    for (const auto& ns_pair : all_templates) {
        if (ns_pair.second.count(template_name)) {
            def = &ns_pair.second.at(template_name);
            break;
        }
    }

    if (def && def->type == TemplateType::STYLE) {
        for (const auto& inherited : def->inherited_templates) {
             resolveStyleTemplate(inherited, all_templates, properties, visited_templates);
        }
        for (const auto& prop : def->style_properties) {
            properties[prop.key] = prop.clone();
        }
    }

    visited_templates.erase(template_name);
}

// This function computes the full style map for a given style node.
inline void resolveStyleNode(
    StyleNode* styleNode,
    const std::map<std::string, std::map<std::string, TemplateDefinitionNode>>& all_templates,
    std::map<std::string, AttributeNode>& final_props)
{
    if (!styleNode) return;

    std::unordered_set<std::string> visited_templates;

    // 1. Apply templates recursively
    for (const auto& app : styleNode->template_applications) {
        resolveStyleTemplate(app.template_name, all_templates, final_props, visited_templates);

        // Apply specializations from the usage (@Style MyTemplate { ... })
        for (const auto& key_to_delete : app.deleted_properties) {
            final_props.erase(key_to_delete);
        }
        for (const auto& prop : app.new_or_overridden_properties) {
            final_props[prop.key] = prop.clone();
        }
    }

    // 2. Apply direct properties
    for (const auto& prop : styleNode->direct_properties) {
        final_props[prop.key] = prop.clone();
    }
}


} // namespace StyleResolver
} // namespace CHTL

#endif // STYLE_RESOLVER_H
