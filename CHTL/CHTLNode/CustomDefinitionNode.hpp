#pragma once

#include "BaseNode.hpp"
#include "TemplateDefinitionNode.hpp" // For TemplateType and StyleRule

namespace CHTL {

class CustomDefinitionNode : public BaseNode {
public:
    void accept(AstVisitor& visitor) override;

    TemplateType customType; // Reusing TemplateType enum (Style, Element, Var)
    std::string name;

    // Body is stored similarly to templates
    std::vector<NodePtr> bodyNodes;
    std::vector<StyleRule> styleRules;
};

} // namespace CHTL
