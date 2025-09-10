#pragma once

#include "BaseNode.h"
#include "ExprNode.h"
#include <string>
#include <vector>
#include <map>

enum class TemplateType {
    STYLE,
    ELEMENT,
    VAR
};

// Represents a template definition like `[Template] @Style MyStyles { ... }`
class TemplateNode : public BaseNode {
public:
    TemplateType template_type;
    std::string name;

    // Content depends on the type
    std::vector<std::pair<std::string, std::unique_ptr<ExprNode>>> style_properties;
    NodeList element_children;
    std::map<std::string, std::string> variables;
    std::vector<std::unique_ptr<class TemplateUsageNode>> inherited_templates;

    TemplateNode(const std::string& name, TemplateType type) : name(name), template_type(type) {}
};

// Represents the usage of a template, e.g., `@Style MyStyles;`
class TemplateUsageNode : public BaseNode {
public:
    TemplateType template_type;
    std::string name;

    // For specialization later
    // NodeList specializations;

    TemplateUsageNode(const std::string& name, TemplateType type) : name(name), template_type(type) {}
};
