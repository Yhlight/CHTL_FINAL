#pragma once

#include "BaseNode.hpp"
#include "StyleNode.hpp" // For StyleRule
#include "../CHTLLexer/Token.hpp"
#include <string>
#include <vector>

namespace CHTL {

enum class TemplateType {
    STYLE,
    ELEMENT,
    VAR
};

class TemplateDefinitionNode : public BaseNode {
public:
    void accept(AstVisitor& visitor) override;

    TemplateType templateType;
    std::string name;

    // For @Element templates, the body is a list of nodes.
    std::vector<NodePtr> bodyNodes;

    // For @Style templates, the body is a list of rules.
    std::vector<StyleRule> styleRules;

    // For @Var templates, we'll need another structure later.
};

} // namespace CHTL
