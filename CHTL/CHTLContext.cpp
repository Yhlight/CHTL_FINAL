#include "CHTLContext.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/StyleNode.h"
#include <unordered_set>
#include <map>

namespace CHTL {

CHTLContext::CHTLContext() : current_namespace("") {}

void CHTLContext::addTemplateDefinition(const TemplateDefinitionNode& def) {
    namespaces[current_namespace].template_definitions[def.name] = def;
}

const TemplateDefinitionNode* CHTLContext::getTemplateDefinition(const std::string& name, const std::string& from_namespace) {
    // If a specific namespace is requested, only look there.
    if (!from_namespace.empty()) {
        auto ns_it = namespaces.find(from_namespace);
        if (ns_it != namespaces.end()) {
            auto& ns_data = ns_it->second;
            auto def_it = ns_data.template_definitions.find(name);
            if (def_it != ns_data.template_definitions.end()) {
                return &def_it->second;
            }
        }
        return nullptr; // Not found in the specified namespace
    }

    // If no namespace is specified, search all of them.
    // This handles the "pseudo-merge" of contexts for imports.
    for (const auto& ns_pair : namespaces) {
        const auto& ns_data = ns_pair.second;
        auto def_it = ns_data.template_definitions.find(name);
        if (def_it != ns_data.template_definitions.end()) {
            return &def_it->second;
        }
    }

    return nullptr;
}


namespace { // Anonymous namespace for helper functions

std::vector<AttributeNode> collectStylesRecursive(CHTLContext& context, const StyleTemplateUsage& usage) {
    const TemplateDefinitionNode* def = context.getTemplateDefinition(usage.name, usage.from_namespace);
    if (!def) {
        return {};
    }

    std::map<std::string, AttributeNode> properties_map;

    // 1. Recursively collect properties from inherited templates
    for (const auto& inherited_usage : def->inherited_styles) {
        std::vector<AttributeNode> base_properties = collectStylesRecursive(context, inherited_usage);
        for (const auto& prop : base_properties) {
            properties_map[prop.key] = prop;
        }
    }

    // 2. Add/overwrite with properties from the current template definition, tagging their origin
    for (const auto& prop : def->style_properties) {
        AttributeNode tagged_prop = prop;
        tagged_prop.origin_template_name = def->name;
        properties_map[tagged_prop.key] = tagged_prop;
    }

    // Convert map back to vector
    std::vector<AttributeNode> final_properties;
    for (const auto& pair : properties_map) {
        final_properties.push_back(pair.second);
    }

    return final_properties;
}

} // anonymous namespace

std::vector<AttributeNode> CHTLContext::resolveStyles(ElementNode* element) {
    if (!element) {
        return {};
    }

    std::map<std::string, AttributeNode> final_props_map;

    for (const auto& child : element->children) {
        if (StyleNode* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            // Process template usages first
            for (const auto& usage : styleNode->template_usages) {
                std::vector<AttributeNode> collected_props = collectStylesRecursive(*this, usage);

                // Apply the top-level usage's specializations
                std::unordered_set<std::string> deleted_templates;
                for(const auto& del_usage : usage.deleted_template_usages) {
                    deleted_templates.insert(del_usage.name);
                }

                for (const auto& prop : collected_props) {
                    if (deleted_templates.find(prop.origin_template_name) == deleted_templates.end()) {
                        final_props_map[prop.key] = prop;
                    }
                }

                for (const auto& prop_to_delete : usage.deleted_properties) {
                    final_props_map.erase(prop_to_delete);
                }

                for (const auto& specialized_prop : usage.specialized_properties) {
                    final_props_map[specialized_prop.key] = specialized_prop;
                }
            }

            // Finally, add inline properties, which have the highest precedence
            for (const auto& prop : styleNode->inline_properties) {
                final_props_map[prop.key] = prop;
            }
        }
    }

    // Convert map to vector for the final result
    std::vector<AttributeNode> result;
    for (const auto& pair : final_props_map) {
        result.push_back(pair.second);
    }
    return result;
}


} // namespace CHTL
