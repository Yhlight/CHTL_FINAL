#pragma once

#include <memory>

namespace CHTL {

// An enum to easily identify node types without needing RTTI (e.g., dynamic_cast).
// This is faster and simpler for this compiler's purposes.
enum class NodeType {
    Document,
    Element,
    Text,
    Attribute,

    // Style-related node types
    StyleBlock,
    StyleProperty,
    StyleSelector,
};

// The abstract base class for all nodes in the CHTL Abstract Syntax Tree.
// It defines a common interface, primarily for identifying the node's specific type.
class BaseNode {
public:
    virtual ~BaseNode() = default;
    virtual NodeType getType() const = 0;
};

} // namespace CHTL
