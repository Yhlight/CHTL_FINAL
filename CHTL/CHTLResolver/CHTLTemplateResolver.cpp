#include "CHTLTemplateResolver.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../CHTLNode/CustomUsageNode.h"
#include "../CHTLNode/CustomDefinitionNode.h"
#include "../CHTLNode/DeletePropertyNode.h"
#include "../CHTLNode/CSSPropertyNode.h"
#include <iostream>
#include <algorithm>

namespace CHTL {

void CHTLTemplateResolver::Process(const std::shared_ptr<ProgramNode>& program) {
    collectTemplates(program);

    for (int i = 0; i < 10; ++i) {
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
            expandTemplates(child);
            if (auto usage = std::dynamic_pointer_cast<TemplateUsageNode>(child)) {
                if (m_template_registry.count(usage->name)) {
                    auto template_def = m_template_registry[usage->name];
                    for (const auto& template_body_node : template_def->body) {
                        new_children.push_back(template_body_node->clone());
                    }
                }
            } else {
                new_children.push_back(child);
            }
        }
        element->children = new_children;
    } else if (auto style = std::dynamic_pointer_cast<StyleNode>(node)) {
         CSSPropertyList props_from_templates;
         NodeList remaining_usages;

         for(const auto& usage_node : style->template_usages) {
             if (auto usage = std::dynamic_pointer_cast<TemplateUsageNode>(usage_node)) {
                  if (m_template_registry.count(usage->name)) {
                    auto template_def = m_template_registry[usage->name];
                    for(const auto& prop_node : template_def->body) {
                        if (auto prop = std::dynamic_pointer_cast<CSSPropertyNode>(prop_node->clone())) {
                            props_from_templates.push_back(prop);
                        }
                    }
                }
             } else if (auto custom_usage = std::dynamic_pointer_cast<CustomUsageNode>(usage_node)) {
                 if (m_template_registry.count(custom_usage->name)) {
                    auto template_def = m_template_registry[custom_usage->name];
                    CSSPropertyList specialized_props;
                    for(const auto& prop_node : template_def->body) {
                        specialized_props.push_back(std::dynamic_pointer_cast<CSSPropertyNode>(prop_node->clone()));
                    }
                    for(const auto& spec_node : custom_usage->body) {
                        if(auto delete_node = std::dynamic_pointer_cast<DeletePropertyNode>(spec_node)) {
                            specialized_props.erase(std::remove_if(specialized_props.begin(), specialized_props.end(),
                                [&](const CSSPropertyPtr& p) { return p->key == delete_node->property_name; }),
                                specialized_props.end());
                        } else if (auto override_prop = std::dynamic_pointer_cast<CSSPropertyNode>(spec_node)) {
                            bool found = false;
                            for(auto& base_prop : specialized_props) {
                                if (base_prop && base_prop->key == override_prop->key) {
                                    if (override_prop->value) {
                                         base_prop->value = std::dynamic_pointer_cast<ExpressionNode>(override_prop->value->clone());
                                    }
                                    found = true;
                                    break;
                                }
                            }
                            if (!found) specialized_props.push_back(std::dynamic_pointer_cast<CSSPropertyNode>(override_prop->clone()));
                        }
                    }
                    props_from_templates.insert(props_from_templates.end(), specialized_props.begin(), specialized_props.end());
                 }
             } else {
                 remaining_usages.push_back(usage_node);
             }
         }
         style->template_usages = remaining_usages;
         style->inline_properties.insert(style->inline_properties.end(), props_from_templates.begin(), props_from_templates.end());

    } else if (auto program = std::dynamic_pointer_cast<ProgramNode>(node)) {
        for(const auto& stmt : program->statements) {
            expandTemplates(stmt);
        }
    }
}

} // namespace CHTL
