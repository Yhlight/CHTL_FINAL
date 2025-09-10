#pragma once

#include "BaseNode.h"
#include "TemplateNode.h" // For TemplateType and TemplateUsageNode
#include "ExprNode.h"
#include <string>
#include <vector>
#include <map>

// Represents a custom definition like `[Custom] @Style MyStyles { ... }`
// It's structurally identical to a TemplateNode for now.
// The difference is in how the generator handles them.
class CustomNode : public BaseNode {
public:
    TemplateType custom_type;
    std::string name;

    // Content depends on the type
    std::vector<std::pair<std::string, std::unique_ptr<ExprNode>>> style_properties;
    NodeList element_children;
    std::map<std::string, std::string> variables;
    std::vector<std::unique_ptr<TemplateUsageNode>> inherited_templates;

    CustomNode(const std::string& name, TemplateType type) : name(name), custom_type(type) {}
};
