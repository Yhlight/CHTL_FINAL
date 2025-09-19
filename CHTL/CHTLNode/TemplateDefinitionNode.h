#ifndef TEMPLATE_DEFINITION_NODE_H
#define TEMPLATE_DEFINITION_NODE_H

#include "BaseNode.h"
#include "AttributeNode.h"
#include "../Expression/Expr.h" // Ensure Expr is fully defined for clone
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <utility>

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

    // Default constructor
    TemplateDefinitionNode() = default;

    // Copy constructor for deep cloning
    TemplateDefinitionNode(const TemplateDefinitionNode& other)
        : type(other.type), name(other.name), is_custom(other.is_custom), style_properties(other.style_properties) {
        for (const auto& item : other.element_body) {
            element_body.push_back(item->clone());
        }
        for (const auto& pair : other.variables) {
            variables[pair.first] = pair.second->clone();
        }
    }

    // Copy assignment operator
    TemplateDefinitionNode& operator=(const TemplateDefinitionNode& other) {
        if (this == &other) {
            return *this;
        }
        type = other.type;
        name = other.name;
        is_custom = other.is_custom;
        style_properties = other.style_properties;

        element_body.clear();
        for (const auto& item : other.element_body) {
            element_body.push_back(item->clone());
        }

        variables.clear();
        for (const auto& pair : other.variables) {
            variables[pair.first] = pair.second->clone();
        }
        return *this;
    }

    // Move constructor
    TemplateDefinitionNode(TemplateDefinitionNode&& other) noexcept = default;

    // Move assignment operator
    TemplateDefinitionNode& operator=(TemplateDefinitionNode&& other) noexcept = default;
};

} // namespace CHTL

#endif // TEMPLATE_DEFINITION_NODE_H
