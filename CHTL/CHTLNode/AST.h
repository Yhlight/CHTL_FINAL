#ifndef AST_H
#define AST_H

#include "../CHTLLexer/Token.h"
#include <string>
#include <vector>
#include <memory>
#include <variant>

// Forward declarations for all AST nodes and the Visitor
class ProgramNode;
class ElementNode;
class AttributeNode;
class TextNode;
class CommentNode;
class ValueNode;
class StringLiteralNode;
class UnquotedLiteralNode;
class NumberLiteralNode;
class StyleBlockNode;
class StylePropertyNode;
class StyleRuleNode;
class Visitor;


// The Visitor interface for processing the AST
class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visit(ProgramNode& node) = 0;
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(AttributeNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
    virtual void visit(CommentNode& node) = 0;
    virtual void visit(StringLiteralNode& node) = 0;
    virtual void visit(UnquotedLiteralNode& node) = 0;
    virtual void visit(NumberLiteralNode& node) = 0;
    virtual void visit(StyleBlockNode& node) = 0;
    virtual void visit(StylePropertyNode& node) = 0;
    virtual void visit(StyleRuleNode& node) = 0;
};


// Base class for all nodes in the AST
class BaseNode {
public:
    virtual ~BaseNode() = default;
    virtual void accept(Visitor& visitor) = 0;
};


// Base class for all value types (strings, numbers, etc.)
class ValueNode : public BaseNode {};

// Represents a quoted string literal, e.g., "hello" or 'world'
class StringLiteralNode : public ValueNode {
public:
    Token value;
    explicit StringLiteralNode(Token value) : value(std::move(value)) {}
    void accept(Visitor& visitor) override;
};

// Represents an unquoted literal, which is a sequence of other tokens
class UnquotedLiteralNode : public ValueNode {
public:
    std::vector<Token> tokens;
    explicit UnquotedLiteralNode(std::vector<Token> tokens) : tokens(std::move(tokens)) {}
    void accept(Visitor& visitor) override;
};

// Represents a number literal
class NumberLiteralNode : public ValueNode {
public:
    Token value;
    explicit NumberLiteralNode(Token value) : value(std::move(value)) {}
    void accept(Visitor& visitor) override;
};


// Represents an element attribute, e.g., id: "box"
class AttributeNode : public BaseNode {
public:
    Token name;
    std::unique_ptr<ValueNode> value;
    AttributeNode(Token name, std::unique_ptr<ValueNode> value)
        : name(std::move(name)), value(std::move(value)) {}
    void accept(Visitor& visitor) override;
};


// Represents an HTML element, e.g., div { ... }
class ElementNode : public BaseNode {
public:
    Token tagName;
    std::vector<std::unique_ptr<AttributeNode>> attributes;
    std::unique_ptr<StyleBlockNode> styleBlock;
    std::vector<std::unique_ptr<BaseNode>> children;

    explicit ElementNode(Token tagName) : tagName(std::move(tagName)), styleBlock(nullptr) {}
    void accept(Visitor& visitor) override;
};


// Represents a text block, e.g., text { ... }
class TextNode : public BaseNode {
public:
    std::unique_ptr<ValueNode> value;
    explicit TextNode(std::unique_ptr<ValueNode> value) : value(std::move(value)) {}
    void accept(Visitor& visitor) override;
};


// Represents a generator comment, e.g., # a comment
class CommentNode : public BaseNode {
public:
    Token comment;
    explicit CommentNode(Token comment) : comment(std::move(comment)) {}
    void accept(Visitor& visitor) override;
};


// The root node of the entire AST
class ProgramNode : public BaseNode {
public:
    std::vector<std::unique_ptr<BaseNode>> children;
    void accept(Visitor& visitor) override;
};


// --- Style-related Nodes ---

// Base class for nodes that can appear inside a style { ... } block.
class StyleContentNode : public BaseNode {};

// Represents a single CSS property, e.g., `width: 100px;`
class StylePropertyNode : public StyleContentNode {
public:
    Token name;
    std::unique_ptr<ValueNode> value;
    StylePropertyNode(Token name, std::unique_ptr<ValueNode> value)
        : name(std::move(name)), value(std::move(value)) {}
    void accept(Visitor& visitor) override;
};

// Represents a full CSS rule with a selector, e.g., `.box { ... }`
class StyleRuleNode : public StyleContentNode {
public:
    std::vector<Token> selector;
    std::vector<std::unique_ptr<StylePropertyNode>> properties;
    explicit StyleRuleNode(std::vector<Token> selector) : selector(std::move(selector)) {}
    void accept(Visitor& visitor) override;
};

// Represents the entire `style { ... }` block associated with an element.
class StyleBlockNode : public BaseNode {
public:
    std::vector<std::unique_ptr<StyleContentNode>> contents;
    void accept(Visitor& visitor) override;
};


#endif // AST_H
