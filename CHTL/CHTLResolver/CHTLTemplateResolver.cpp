#include "CHTLTemplateResolver.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include <iostream>

namespace CHTL {

// A simple clone function for nodes. This is non-trivial.
// For now, we will return the same shared_ptr, which is NOT a deep copy.
// This means a template can only be used once.
// A full implementation would require a deep-copying clone method on each node type.
NodePtr cloneNode(const NodePtr& node) {
    // THIS IS A SHALLOW COPY FOR NOW
    return node;
}

NodeList cloneNodeList(const NodeList& list) {
    // THIS IS A SHALLOW COPY FOR NOW
    return list;
}


void CHTLTemplateResolver::Process(const std::shared_ptr<ProgramNode>& program) {
    // Pass 1: Collect all template definitions
    collectTemplates(program);

    // Pass 2: Expand all template usages
    auto expanded_program_node = expandTemplates(program);
    if (auto expanded_program = std::dynamic_pointer_cast<ProgramNode>(expanded_program_node)) {
        program->statements = expanded_program->statements;
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

NodePtr CHTLTemplateResolver::expandTemplates(const NodePtr& node) {
    if (!node) return nullptr;

    // Expand children first (post-order traversal)
    if (auto element = std::dynamic_pointer_cast<ElementNode>(node)) {
        NodeList new_children;
        for(const auto& child : element->children) {
            auto expanded_child = expandTemplates(child);
            if (auto usage = std::dynamic_pointer_cast<TemplateUsageNode>(expanded_child)) {
                if (m_template_registry.count(usage->name)) {
                    auto template_def = m_template_registry[usage->name];
                    // IMPORTANT: This is a shallow copy. Using a template multiple times will fail.
                    new_children.insert(new_children.end(), template_def->body.begin(), template_def->body.end());
                }
            } else {
                new_children.push_back(expanded_child);
            }
        }
        element->children = new_children;
    } else if (auto style = std::dynamic_pointer_cast<StyleNode>(node)) {
         NodeList new_usages;
         for(const auto& usage_node : style->template_usages) {
             auto expanded_usage = expandTemplates(usage_node);
             if (auto usage = std::dynamic_pointer_cast<TemplateUsageNode>(expanded_usage)) {
                  if (m_template_registry.count(usage->name)) {
                    auto template_def = m_template_registry[usage->name];
                    // Replace usage with properties from the template body
                    for(const auto& prop_node : template_def->body) {
                        if (auto prop = std::dynamic_pointer_cast<CSSPropertyNode>(prop_node)) {
                            style->inline_properties.push_back(prop);
                        }
                    }
                }
             }
         }
         style->template_usages.clear(); // Clear usages as they are now expanded
    } else if (auto program = std::dynamic_pointer_cast<ProgramNode>(node)) {
        NodeList new_statements;
        for(const auto& stmt : program->statements) {
            new_statements.push_back(expandTemplates(stmt));
        }
        program->statements = new_statements;
    }

    return node;
}


} // namespace CHTL
