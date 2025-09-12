#ifndef CHTL_BASE_NODE_H
#define CHTL_BASE_NODE_H

#include <memory>
#include <vector>
#include <string>

namespace CHTL {

enum class NodeType {
    // Core nodes
    Element,
    Text,
    Comment,
    Attribute,

    // Root of the document
    Root,

    // CHTL specific constructs
    StyleBlock,
    CssRule,
    PropertyReference,
    ScriptBlock,
    Template,
    Custom,
    Origin,
    Import,
    Namespace,
    Configuration
};

class Node {
public:
    virtual ~Node() = default;
    virtual NodeType getType() const = 0;
    virtual std::unique_ptr<Node> clone() const = 0;
};

} // namespace CHTL

#endif // CHTL_BASE_NODE_H
