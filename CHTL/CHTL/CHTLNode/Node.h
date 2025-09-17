#pragma once

#include <string>
#include <vector>
#include <memory>

// Forward declarations for node types
struct ElementNode;
struct TextNode;
struct AttributeNode;

// Base class for all AST nodes
struct Node {
    virtual ~Node() = default;
};

// Represents an attribute (e.g., id="main")
struct AttributeNode : public Node {
    std::string name;
    std::string value;
};

// Represents an element (e.g., div { ... })
struct ElementNode : public Node {
    std::string tagName;
    std::vector<std::shared_ptr<AttributeNode>> attributes;
    std::vector<std::shared_ptr<Node>> children; // Children can be Elements or Text
};

// Represents a text block (e.g., text { "Hello" })
struct TextNode : public Node {
    std::string content;
};

// The root of the AST, containing all top-level statements
struct RootNode : public Node {
    std::vector<std::shared_ptr<Node>> statements;
};
