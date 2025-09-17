#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../CHTLLexer/Token.h"

// Forward declarations
struct ProgramNode;
struct ElementNode;
struct AttributeNode;
struct TextNode;
struct StyleNode;
struct ValueNode;
struct SelectorNode; // New

// Base Node class using visitor pattern for future operations (like generation)
struct INodeVisitor;

struct Node {
    virtual ~Node() = default;
    virtual void accept(INodeVisitor& visitor) = 0;
};

// Visitor Interface
struct INodeVisitor {
    virtual void visit(ProgramNode& node) = 0;
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(AttributeNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
    virtual void visit(StyleNode& node) = 0;
    virtual void visit(ValueNode& node) = 0;
    virtual void visit(SelectorNode& node) = 0; // New
};


// Represents a literal value
struct ValueNode : public Node {
    Token token; // The token containing the value (e.g., STRING_LITERAL, UNQUOTED_LITERAL)

    explicit ValueNode(const Token& token) : token(token) {}

    void accept(INodeVisitor& visitor) override {
        visitor.visit(*this);
    }
};

// Represents an attribute (e.g., id: "main")
struct AttributeNode : public Node {
    std::string key;
    std::unique_ptr<ValueNode> value;

    AttributeNode(const std::string& key, std::unique_ptr<ValueNode> value)
        : key(key), value(std::move(value)) {}

    void accept(INodeVisitor& visitor) override {
        visitor.visit(*this);
    }
};

// Represents a style block (e.g., style { color: red; })
// Can now contain direct properties (AttributeNode) or full selector blocks (SelectorNode)
struct StyleNode : public Node {
    std::vector<std::unique_ptr<Node>> children;

    void accept(INodeVisitor& visitor) override {
        visitor.visit(*this);
    }
};

// Represents a CSS selector block (e.g., .box { color: red; })
struct SelectorNode : public Node {
    std::string selector;
    std::vector<std::unique_ptr<AttributeNode>> properties;

    explicit SelectorNode(const std::string& selector) : selector(selector) {}

    void accept(INodeVisitor& visitor) override {
        visitor.visit(*this);
    }
};

// Represents a text block (e.g., text { "hello" })
struct TextNode : public Node {
    std::unique_ptr<ValueNode> content;

    explicit TextNode(std::unique_ptr<ValueNode> content) : content(std::move(content)) {}

    void accept(INodeVisitor& visitor) override {
        visitor.visit(*this);
    }
};

// Represents an element (e.g., div { ... })
struct ElementNode : public Node {
    std::string tagName;
    std::vector<std::unique_ptr<Node>> children; // Can contain AttributeNode, ElementNode, TextNode, StyleNode

    explicit ElementNode(const std::string& tagName) : tagName(tagName) {}

    void accept(INodeVisitor& visitor) override {
        visitor.visit(*this);
    }
};

// The root of the AST
struct ProgramNode : public Node {
    std::vector<std::unique_ptr<Node>> children;

    void accept(INodeVisitor& visitor) override {
        visitor.visit(*this);
    }
};
