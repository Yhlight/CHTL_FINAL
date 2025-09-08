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
    InsertRule,

    // New types for Import/Namespace
    Import,
    Namespace,

    // Variables
    VariableDefinition,
    VariableGroupDefinition,

    // Raw Embedding
    Origin,
    OriginUsage,
};

class BaseNode {
public:
    virtual ~BaseNode() = default;
    virtual NodeType getType() const = 0;
    virtual std::unique_ptr<BaseNode> clone() const = 0;
};

} // namespace CHTL
