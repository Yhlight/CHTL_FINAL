#pragma once

#include "../CHTLLexer/Token.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {

// Forward declarations
class Node;
class ExpressionNode;
class NumberLiteralNode;
class StringLiteralNode;
class BinaryExpressionNode;
class ConditionalExpressionNode;
class StylePropertyNode;
class CommentNode;
class TextNode;
class InheritanceNode;
class TemplateDefinitionNode;
class ElementNode;

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

//== Expression Nodes ==//

class ExpressionNode : public Node {};

class NumberLiteralNode : public ExpressionNode {
public:
    int value;
    explicit NumberLiteralNode(int v) : value(v) {}

    std::string toString() const override { return std::to_string(value); }
    std::unique_ptr<Node> clone() const override { return std::make_unique<NumberLiteralNode>(value); }
};

class BinaryExpressionNode : public ExpressionNode {
public:
    std::unique_ptr<ExpressionNode> left;
    TokenType op;
    std::unique_ptr<ExpressionNode> right;

    BinaryExpressionNode(std::unique_ptr<ExpressionNode> l, TokenType o, std::unique_ptr<ExpressionNode> r)
        : left(std::move(l)), op(o), right(std::move(r)) {}

    std::string toString() const override {
        return "(" + left->toString() + " " + tokenTypeToString(op) + " " + right->toString() + ")";
    }
    std::unique_ptr<Node> clone() const override {
        return std::make_unique<BinaryExpressionNode>(
            std::unique_ptr<ExpressionNode>(static_cast<ExpressionNode*>(left->clone().release())),
            op,
            std::unique_ptr<ExpressionNode>(static_cast<ExpressionNode*>(right->clone().release()))
        );
    }
};

class StringLiteralNode : public ExpressionNode {
public:
    std::string value;
    explicit StringLiteralNode(std::string v) : value(std::move(v)) {}

    std::string toString() const override { return "\"" + value + "\""; }
    std::unique_ptr<Node> clone() const override { return std::make_unique<StringLiteralNode>(value); }
};

class ConditionalExpressionNode : public ExpressionNode {
public:
    std::unique_ptr<ExpressionNode> condition;
    std::unique_ptr<ExpressionNode> consequence;
    std::unique_ptr<ExpressionNode> alternative;

    ConditionalExpressionNode(std::unique_ptr<ExpressionNode> c, std::unique_ptr<ExpressionNode> cons, std::unique_ptr<ExpressionNode> alt)
        : condition(std::move(c)), consequence(std::move(cons)), alternative(std::move(alt)) {}

    std::string toString() const override {
        return "(" + condition->toString() + " ? " + consequence->toString() + " : " + alternative->toString() + ")";
    }
    std::unique_ptr<Node> clone() const override {
        return std::make_unique<ConditionalExpressionNode>(
            std::unique_ptr<ExpressionNode>(static_cast<ExpressionNode*>(condition->clone().release())),
            std::unique_ptr<ExpressionNode>(static_cast<ExpressionNode*>(consequence->clone().release())),
            std::unique_ptr<ExpressionNode>(static_cast<ExpressionNode*>(alternative->clone().release()))
        );
    }
};

//== General AST Nodes ==//

class CommentNode : public Node {
public:
    std::string value;
    explicit CommentNode(std::string val) : value(std::move(val)) {}
    std::string toString() const override { return "<!-- " + value + " -->"; }
    std::unique_ptr<Node> clone() const override { return std::make_unique<CommentNode>(value); }
};

class TextNode : public Node {
public:
    std::string value;
    explicit TextNode(std::string val) : value(std::move(val)) {}
    std::string toString() const override { return value; }
    std::unique_ptr<Node> clone() const override { return std::make_unique<TextNode>(value); }
};

class StylePropertyNode : public Node {
public:
    std::string key;
    std::unique_ptr<ExpressionNode> value;

    StylePropertyNode(std::string k, std::unique_ptr<ExpressionNode> v) : key(std::move(k)), value(std::move(v)) {}

    std::string toString() const override {
        return key + ":" + value->toString() + ";";
    }

    std::unique_ptr<Node> clone() const override {
        return std::make_unique<StylePropertyNode>(
            key,
            std::unique_ptr<ExpressionNode>(static_cast<ExpressionNode*>(value->clone().release()))
        );
    }
};

class InheritanceNode : public Node {
public:
    TemplateType type;
    std::string name;

    InheritanceNode(TemplateType t, std::string n) : type(t), name(std::move(n)) {}
    std::string toString() const override { return "inherit @" + name; }
    std::unique_ptr<Node> clone() const override { return std::make_unique<InheritanceNode>(type, name); }
};

class TemplateDefinitionNode : public Node {
public:
    TemplateType type;
    std::string name;
    std::vector<std::unique_ptr<Node>> children;

    TemplateDefinitionNode(TemplateType t, std::string n) : type(t), name(std::move(n)) {}
    void addChild(std::unique_ptr<Node> child) { children.push_back(std::move(child)); }
    std::string toString() const override { return "[Template] @" + name; }
    std::unique_ptr<Node> clone() const override {
        auto newClone = std::make_unique<TemplateDefinitionNode>(type, name);
        for (const auto& child : children) {
            newClone->addChild(child->clone());
        }
        return newClone;
    }
};

class ElementNode : public Node {
public:
    std::string tagName;
    std::map<std::string, std::string> attributes;
    std::map<std::string, std::string> styles;
    std::vector<std::unique_ptr<Node>> children;

    explicit ElementNode(std::string name) : tagName(std::move(name)) {}
    void addChild(std::unique_ptr<Node> child) { children.push_back(std::move(child)); }
    std::string toString() const override {
        std::string out = "<" + tagName;
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

class Program {
public:
    std::vector<std::unique_ptr<Node>> children;
    void addChild(std::unique_ptr<Node> child) { children.push_back(std::move(child)); }
    std::string toString() const {
        std::string out;
        for (const auto& child : children) {
            out += child->toString();
        }
        return out;
    }
};

} // namespace CHTL
