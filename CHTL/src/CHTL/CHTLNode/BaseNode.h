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

    // Specialization Rule Nodes
    DeleteRule,
    InsertRule,
};

class BaseNode {
public:
    virtual ~BaseNode() = default;
    virtual NodeType getType() const = 0;
    virtual std::unique_ptr<BaseNode> clone() const = 0;
};

} // namespace CHTL
