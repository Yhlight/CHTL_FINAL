#pragma once

#include <memory>

namespace CHTL {

enum class NodeType {
    Document,
    Element,
    Text,
    Attribute,

    StyleBlock,
    StyleProperty,
    StyleSelector,

    TemplateDefinition,
    TemplateUsage,

    CustomDefinition,
    CustomUsage,

    DeleteRule,
};

class BaseNode {
public:
    virtual ~BaseNode() = default;
    virtual NodeType getType() const = 0;
    // Add a virtual clone method for deep copying the AST
    virtual std::unique_ptr<BaseNode> clone() const = 0;
};

} // namespace CHTL
