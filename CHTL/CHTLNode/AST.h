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
struct StyleTemplateDefinitionNode;
struct StyleTemplateUsageNode;
struct ImportNode;
struct AttributeNode;

// Base class for all AST nodes
struct BaseNode {
    virtual ~BaseNode() = default;
    virtual void accept(Visitor& visitor) = 0;
};

// Represents an attribute (e.g., id: "box").
struct AttributeNode {
    std::string key;
    std::string value;
};

// Represents a single CSS property (e.g., color: red;)
struct CssPropertyNode : public BaseNode {
    std::string key;
    std::string value;
    void accept(Visitor& visitor) override;
};

// Represents a [Template] @Style ... {} definition
struct StyleTemplateDefinitionNode : public BaseNode {
    std::string name;
    std::vector<CssPropertyNode> properties;
    void accept(Visitor& visitor) override;
};

// Represents a @Style ...; usage
struct StyleTemplateUsageNode : public BaseNode {
    std::string name;
    void accept(Visitor& visitor) override;
};

// Represents a style { ... } block
struct StyleBlockNode : public BaseNode {
    std::vector<std::unique_ptr<BaseNode>> items; // Can contain CssPropertyNode or StyleTemplateUsageNode
    void accept(Visitor& visitor) override;
};

// Visitor base class
class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
    virtual void visit(StyleBlockNode& node) = 0;
    virtual void visit(CssPropertyNode& node) = 0;
    virtual void visit(StyleTemplateDefinitionNode& node) = 0;
    virtual void visit(StyleTemplateUsageNode& node) = 0;
    virtual void visit(ImportNode& node) = 0;
};

// Represents an [Import] ... from "..." statement
struct ImportNode : public BaseNode {
    std::string path;
    void accept(Visitor& visitor) override {
        visitor.visit(*this);
    }
};

// --- Inline implementations of accept methods ---

inline void CssPropertyNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

inline void StyleTemplateDefinitionNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

inline void StyleTemplateUsageNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

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
