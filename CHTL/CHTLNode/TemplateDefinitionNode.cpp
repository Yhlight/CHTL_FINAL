#include "TemplateDefinitionNode.h"
#include "../Expression/Expr.h"

namespace CHTL {

TemplateDefinitionNode::TemplateDefinitionNode(const TemplateDefinitionNode& other) {
    type = other.type;
    name = other.name;
    for (const auto& node : other.element_body) {
        element_body.push_back(node->clone());
    }
    for (const auto& prop : other.style_properties) {
        style_properties.push_back(prop.clone());
    }
    for (const auto& pair : other.variables) {
        variables[pair.first] = pair.second->clone();
    }
}

TemplateDefinitionNode& TemplateDefinitionNode::operator=(const TemplateDefinitionNode& other) {
    if (this == &other) {
        return *this;
    }
    type = other.type;
    name = other.name;
    element_body.clear();
    for (const auto& node : other.element_body) {
        element_body.push_back(node->clone());
    }
    style_properties = other.style_properties;
    variables.clear();
    for (const auto& pair : other.variables) {
        variables[pair.first] = pair.second->clone();
    }
    return *this;
}

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

    return new_def;
}

} // namespace CHTL
