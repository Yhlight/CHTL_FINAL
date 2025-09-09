#pragma once

#include <string>
#include <vector>
#include <memory>

namespace CHTL {

// Forward declarations
struct ElementNode;
struct TextNode;
struct AttributeNode;

// Base class for all AST nodes
struct BaseNode {
    virtual ~BaseNode() = default;
};

// Represents an attribute (e.g., id: "box")
struct AttributeNode : public BaseNode {
    std::string key;
    std::string value;
};

// Represents a text block
struct TextNode : public BaseNode {
    std::string content;
};

// Represents an element (e.g., div { ... })
struct ElementNode : public BaseNode {
    std::string tagName;
    std::vector<AttributeNode> attributes;
    std::vector<std::unique_ptr<BaseNode>> children;
};

// The root of the AST will be a list of top-level nodes
using DocumentNode = std::vector<std::unique_ptr<BaseNode>>;

} // namespace CHTL
