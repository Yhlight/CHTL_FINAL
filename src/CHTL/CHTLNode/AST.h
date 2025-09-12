#pragma once

#include <string>
#include <vector>
#include <memory>

namespace CHTL {

// Forward declarations for clarity
struct Node;
struct ProgramNode;
struct ElementNode;
struct TextNode;
struct AttributeNode;
struct CommentNode;

// --- Base Node Interface ---
// All AST nodes will derive from this struct.
struct Node {
    virtual ~Node() = default;
};

// --- Node Implementations ---

// Represents an attribute of an element, e.g., id: "box"
struct AttributeNode : public Node {
    std::string name;
    std::string value;
};

// Represents a generator-aware comment, e.g., -- my comment
// These will be translated into HTML comments.
struct CommentNode : public Node {
    std::string text;
};

// Represents a block of text, e.g., text { "content" }
struct TextNode : public Node {
    std::string content;
};

// Represents a CHTL element, e.g., div { ... }
// It can contain attributes and other nodes as children.
struct ElementNode : public Node {
    std::string tagName;
    std::vector<std::shared_ptr<AttributeNode>> attributes;
    std::vector<std::shared_ptr<Node>> children;
};

// Represents the root of a CHTL document.
// It contains a list of all top-level nodes.
struct ProgramNode : public Node {
    std::vector<std::shared_ptr<Node>> children;
};

} // namespace CHTL
