#pragma once

#include <string>
#include <vector>
#include <memory>

namespace CHTL {

// Forward declarations for the Visitor pattern
class Visitor;
struct ElementNode;
struct TextNode;
struct StyleBlockNode;
struct CssPropertyNode;
struct AttributeNode;

// Base class for all AST nodes
struct BaseNode {
    virtual ~BaseNode() = default;
    virtual void accept(Visitor& visitor) = 0;
};

// Represents an attribute (e.g., id: "box").
// Not directly visited in the main tree traversal, but can be part of a visit.
struct AttributeNode {
    std::string key;
    std::string value;
};

// Represents a single CSS property (e.g., color: red;)
struct CssPropertyNode {
    std::string key;
    std::string value;
};

// Represents a style { ... } block
struct StyleBlockNode : public BaseNode {
    std::vector<CssPropertyNode> properties;
    void accept(Visitor& visitor) override; // Implementation will need to be in a cpp file or defined here
};

// Visitor base class
class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
    virtual void visit(StyleBlockNode& node) = 0;
};

// We must define the accept method for StyleBlockNode now that Visitor is fully defined.
inline void StyleBlockNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

// Represents a text block
struct TextNode : public BaseNode {
    std::string content;
    void accept(Visitor& visitor) override {
        visitor.visit(*this);
    }
};

// Represents an element (e.g., div { ... })
struct ElementNode : public BaseNode {
    std::string tagName;
    std::vector<AttributeNode> attributes;
    std::vector<std::unique_ptr<BaseNode>> children;

    void accept(Visitor& visitor) override {
        visitor.visit(*this);
    }
};

// The root of the AST will be a list of top-level nodes
using DocumentNode = std::vector<std::unique_ptr<BaseNode>>;

} // namespace CHTL
