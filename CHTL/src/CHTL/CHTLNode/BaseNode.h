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

    // Template-related node types
    TemplateDefinition,
    TemplateUsage,
};

class BaseNode {
public:
    virtual ~BaseNode() = default;
    virtual NodeType getType() const = 0;
};

} // namespace CHTL
