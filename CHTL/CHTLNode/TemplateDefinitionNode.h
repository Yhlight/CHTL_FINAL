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

    std::unique_ptr<TemplateDefinitionNode> clone() const {
        auto new_node = std::make_unique<TemplateDefinitionNode>();
        new_node->type = this->type;
        new_node->name = this->name;
        new_node->is_custom = this->is_custom;

        for (const auto& child : this->element_body) {
            new_node->element_body.push_back(child->clone());
        }
        for (const auto& prop : this->style_properties) {
             new_node->style_properties.push_back(prop.clone());
        }
        for (const auto& var : this->variables) {
            new_node->variables[var.first] = var.second->clone();
        }
        return new_node;
    }
};

} // namespace CHTL

#endif // TEMPLATE_DEFINITION_NODE_H
