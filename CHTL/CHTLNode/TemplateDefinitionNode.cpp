#include "TemplateDefinitionNode.h"
#include "../Expression/Expr.h"

namespace CHTL {

std::unique_ptr<TemplateDefinitionNode> TemplateDefinitionNode::clone() const {
    auto new_def = std::make_unique<TemplateDefinitionNode>();
    new_def->type = this->type;
    new_def->name = this->name;

    // Clone element body
    for (const auto& node : this->element_body) {
        new_def->element_body.push_back(node->clone());
    }

    // Clone style properties
    for (const auto& prop : this->style_properties) {
        new_def->style_properties.push_back(prop.clone());
    }

    // Clone variables
    for (const auto& pair : this->variables) {
        new_def->variables[pair.first] = pair.second->clone();
    }

    // Clone deleted properties
    new_def->deleted_properties = this->deleted_properties;

    return new_def;
}

} // namespace CHTL
