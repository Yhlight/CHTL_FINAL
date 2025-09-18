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
    VAR // Not used yet, but for future expansion
};

// This class is a data structure used by the parser to store a template's
// definition. It is NOT part of the main document AST and does not inherit
// from BaseNode.
class TemplateDefinitionNode {
public:
    TemplateType type;
    std::string name;

    // Used for @Element templates
    std::vector<std::unique_ptr<BaseNode>> element_body;

    // Used for @Style templates
    std::vector<AttributeNode> style_properties;

    // Used for @Var templates
    std::map<std::string, std::unique_ptr<Expr>> variables;
};

} // namespace CHTL

#endif // TEMPLATE_DEFINITION_NODE_H
