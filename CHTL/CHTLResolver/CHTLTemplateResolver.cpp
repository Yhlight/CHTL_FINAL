#include "CHTLTemplateResolver.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../CHTLNode/CSSPropertyNode.h"
#include <iostream>

namespace CHTL {

void CHTLTemplateResolver::Process(const std::shared_ptr<ProgramNode>& program) {
    collectTemplates(program);

    // Keep expanding until no usages are left. This is a simple but effective way
    // to handle nested templates without complex recursion.
    // A more efficient implementation might track if an expansion occurred.
    for (int i = 0; i < 10; ++i) { // Limit to 10 passes to prevent infinite loops
         expandTemplates(program);
    }
}

void CHTLTemplateResolver::collectTemplates(const NodePtr& node) {
    if (!node) return;
    if (auto program = std::dynamic_pointer_cast<ProgramNode>(node)) {
        NodeList remaining_statements;
        for (const auto& stmt : program->statements) {
            if (auto templateDef = std::dynamic_pointer_cast<TemplateDefinitionNode>(stmt)) {
                m_template_registry[templateDef->name] = templateDef;
            } else {
                remaining_statements.push_back(stmt);
            }
        }
        program->statements = remaining_statements;
    }
}

void CHTLTemplateResolver::expandTemplates(const NodePtr& node) {
    if (!node) return;

    if (auto element = std::dynamic_pointer_cast<ElementNode>(node)) {
        NodeList new_children;
        for(const auto& child : element->children) {
            if (auto usage = std::dynamic_pointer_cast<TemplateUsageNode>(child)) {
                if (m_template_registry.count(usage->name)) {
                    auto template_def = m_template_registry[usage->name];
                    for (const auto& template_body_node : template_def->body) {
                        new_children.push_back(template_body_node->clone());
                    }
                }
            } else {
                expandTemplates(child); // Recurse into non-usage children
                new_children.push_back(child);
            }
        }
        element->children = new_children;
    } else if (auto style = std::dynamic_pointer_cast<StyleNode>(node)) {
         for(const auto& usage_node : style->template_usages) {
             if (auto usage = std::dynamic_pointer_cast<TemplateUsageNode>(usage_node)) {
                  if (m_template_registry.count(usage->name)) {
                    auto template_def = m_template_registry[usage->name];
                    for(const auto& prop_node : template_def->body) {
                        if (auto prop = std::dynamic_pointer_cast<CSSPropertyNode>(prop_node->clone())) {
                            style->inline_properties.push_back(prop);
                        }
                    }
                }
             }
         }
         style->template_usages.clear();
    } else if (auto program = std::dynamic_pointer_cast<ProgramNode>(node)) {
        for(const auto& stmt : program->statements) {
            expandTemplates(stmt);
        }
    }
}

} // namespace CHTL
