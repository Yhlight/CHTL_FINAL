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
class TemplateDefinitionNode;
class StylePropertyNode;
class InheritanceNode;

enum class TemplateType {
    STYLE,
    ELEMENT,
    VAR
};

// Base class for all nodes in the AST
class Node {
public:
    virtual ~Node() = default;
    virtual std::string toString() const = 0;
    virtual std::unique_ptr<Node> clone() const = 0;
};

// Represents a comment node, e.g., # This is a comment
class CommentNode : public Node {
public:
    std::string value;

    explicit CommentNode(std::string val) : value(std::move(val)) {}

    std::string toString() const override {
        return "<!-- " + value + " -->";
    }

    std::unique_ptr<Node> clone() const override {
        return std::make_unique<CommentNode>(value);
    }
};

// Represents a key:value style property
class StylePropertyNode : public Node {
public:
    std::string key;
    std::string value;

    StylePropertyNode(std::string k, std::string v) : key(std::move(k)), value(std::move(v)) {}

    std::string toString() const override {
        return key + ":" + value + ";";
    }

    std::unique_ptr<Node> clone() const override {
        return std::make_unique<StylePropertyNode>(key, value);
    }
};

// Represents an inheritance statement, e.g., inherit @Style ThemeColor;
class InheritanceNode : public Node {
public:
    TemplateType type;
    std::string name;

    InheritanceNode(TemplateType t, std::string n) : type(t), name(std::move(n)) {}

    std::string toString() const override {
        return "inherit @" + name;
    }

    std::unique_ptr<Node> clone() const override {
        return std::make_unique<InheritanceNode>(type, name);
    }
};

// Represents a [Template] definition block
class TemplateDefinitionNode : public Node {
public:
    TemplateType type;
    std::string name;
    std::vector<std::unique_ptr<Node>> children;

    TemplateDefinitionNode(TemplateType t, std::string n) : type(t), name(std::move(n)) {}

    void addChild(std::unique_ptr<Node> child) {
        children.push_back(std::move(child));
    }

    std::string toString() const override {
        // For debugging, not for final generation
        return "[Template] @" + name;
    }

    std::unique_ptr<Node> clone() const override {
        auto newClone = std::make_unique<TemplateDefinitionNode>(type, name);
        for (const auto& child : children) {
            newClone->addChild(child->clone());
        }
        return newClone;
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

    std::unique_ptr<Node> clone() const override {
        return std::make_unique<TextNode>(value);
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

    std::unique_ptr<Node> clone() const override {
        auto newClone = std::make_unique<ElementNode>(tagName);
        newClone->attributes = attributes;
        newClone->styles = styles;
        for (const auto& child : children) {
            newClone->addChild(child->clone());
        }
        return newClone;
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
