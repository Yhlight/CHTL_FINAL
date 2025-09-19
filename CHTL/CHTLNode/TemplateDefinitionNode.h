#ifndef TEMPLATE_DEFINITION_NODE_H
#define TEMPLATE_DEFINITION_NODE_H

#include "BaseNode.h"
#include "AttributeNode.h"
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace CHTL {

enum class TemplateType {
    ELEMENT,
    STYLE,
    VAR
};

class TemplateDefinitionNode {
public:
    TemplateType type;
    std::string name;
    bool is_custom = false;

    std::vector<std::unique_ptr<BaseNode>> element_body;
    std::vector<AttributeNode> style_properties;
    std::map<std::string, std::unique_ptr<Expr>> variables;
    std::vector<std::string> parent_templates;

    TemplateDefinitionNode clone() const {
        TemplateDefinitionNode new_def;
        new_def.type = type;
        new_def.name = name;
        new_def.is_custom = is_custom;
        new_def.parent_templates = parent_templates;

        for (const auto& prop : style_properties) {
            new_def.style_properties.push_back(prop.clone());
        }
        for (const auto& node : element_body) {
            new_def.element_body.push_back(node->clone());
        }
        for (const auto& var : variables) {
            new_def.variables[var.first] = var.second->clone();
        }
        return new_def;
    }
};

} // namespace CHTL

#endif // TEMPLATE_DEFINITION_NODE_H
