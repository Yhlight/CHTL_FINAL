#ifndef CUSTOM_DEFINITION_NODE_H
#define CUSTOM_DEFINITION_NODE_H

#include "BaseNode.h"
#include "AttributeNode.h"
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace CHTL {

enum class CustomType {
    ELEMENT,
    STYLE,
    VAR
};

// This class is a data structure used by the parser to store a custom block's
// definition. It is NOT part of the main document AST.
class CustomDefinitionNode {
public:
    CustomType type;
    std::string name;

    // Used for @Element custom blocks
    std::vector<std::unique_ptr<BaseNode>> element_body;

    // Used for @Style custom blocks
    std::vector<AttributeNode> style_properties;
    std::vector<std::string> valueless_style_properties;

    // Used for @Var custom blocks
    std::map<std::string, std::unique_ptr<Expr>> variables;
};

} // namespace CHTL

#endif // CUSTOM_DEFINITION_NODE_H
