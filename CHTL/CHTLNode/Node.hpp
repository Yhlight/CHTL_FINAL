#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {

// Forward declarations
class ElementNode;
class TextNode;
class CommentNode;

// Base class for all nodes in the AST
class Node {
public:
    virtual ~Node() = default;
    virtual std::string toString() const = 0;
};

// Represents a comment node, e.g., # This is a comment
class CommentNode : public Node {
public:
    std::string value;

    explicit CommentNode(std::string val) : value(std::move(val)) {}

    std::string toString() const override {
        return "<!-- " + value + " -->";
    }
};

// Represents a text literal node
class TextNode : public Node {
public:
    std::string value;

    explicit TextNode(std::string val) : value(std::move(val)) {}

    std::string toString() const override {
        return value;
    }
};

// Represents an element node, e.g., <div>...</div>
class ElementNode : public Node {
public:
    std::string tagName;
    std::map<std::string, std::string> attributes;
    std::map<std::string, std::string> styles;
    std::vector<std::unique_ptr<Node>> children;

    explicit ElementNode(std::string name) : tagName(std::move(name)) {}

    void addChild(std::unique_ptr<Node> child) {
        children.push_back(std::move(child));
    }

    std::string toString() const override {
        std::string out = "<" + tagName;
        // In a real generator, we would handle attributes here.
        out += ">";
        for (const auto& child : children) {
            out += child->toString();
        }
        out += "</" + tagName + ">";
        return out;
    }
};

// The root of the AST is just a node, typically an ElementNode (e.g., <html>)
class Program {
public:
    std::vector<std::unique_ptr<Node>> children;

    void addChild(std::unique_ptr<Node> child) {
        children.push_back(std::move(child));
    }

    std::string toString() const {
        std::string out;
        for (const auto& child : children) {
            out += child->toString();
        }
        return out;
    }
};


} // namespace CHTL
