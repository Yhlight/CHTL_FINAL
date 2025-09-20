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
    NONE,
    ELEMENT,
    STYLE,
    VAR,
    CHTL
};

class TemplateDefinitionNode {
public:
    TemplateType type;
    std::string name;
    bool is_custom = false;

    std::vector<std::unique_ptr<BaseNode>> element_body;
    std::vector<AttributeNode> style_properties;
    std::map<std::string, std::unique_ptr<Expr>> variables;
};

} // namespace CHTL

#endif // TEMPLATE_DEFINITION_NODE_H
