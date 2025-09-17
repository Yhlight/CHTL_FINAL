#ifndef AST_H
#define AST_H

#include "../CHTLLexer/Token.h"
#include <string>
#include <vector>
#include <memory>
#include <variant>

// Forward declarations
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
class TemplateDefinitionNode;
class TemplateStyleNode;
class TemplateElementNode;
class UseStyleNode;
class UseElementNode;
class ExpressionNode;
class BinaryOpNode;
class DimensionNode;
class Visitor;


// The Visitor interface
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
    virtual void visit(TemplateStyleNode& node) = 0;
    virtual void visit(TemplateElementNode& node) = 0;
    virtual void visit(UseStyleNode& node) = 0;
    virtual void visit(UseElementNode& node) = 0;
    virtual void visit(BinaryOpNode& node) = 0;
    virtual void visit(DimensionNode& node) = 0;
};


// Base class for all nodes
class BaseNode {
public:
    virtual ~BaseNode() = default;
    virtual void accept(Visitor& visitor) = 0;
};

// --- Expression Node Hierarchy (for style properties) ---
class ExpressionNode : public BaseNode {};

class BinaryOpNode : public ExpressionNode {
public:
    std::unique_ptr<ExpressionNode> left;
    Token op;
    std::unique_ptr<ExpressionNode> right;
    BinaryOpNode(std::unique_ptr<ExpressionNode> left, Token op, std::unique_ptr<ExpressionNode> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}
    void accept(Visitor& visitor) override;
};

class DimensionNode : public ExpressionNode {
public:
    Token number;
    Token unit; // can be empty
    DimensionNode(Token number, Token unit) : number(std::move(number)), unit(std::move(unit)) {}
    void accept(Visitor& visitor) override;
};


// --- Legacy Value Node Hierarchy (for attributes and text) ---
class ValueNode : public BaseNode {};

class StringLiteralNode : public ValueNode {
public:
    Token value;
    explicit StringLiteralNode(Token value) : value(std::move(value)) {}
    void accept(Visitor& visitor) override;
};

class UnquotedLiteralNode : public ValueNode {
public:
    std::vector<Token> tokens;
    explicit UnquotedLiteralNode(std::vector<Token> tokens) : tokens(std::move(tokens)) {}
    void accept(Visitor& visitor) override;
};

// Number literals are now part of the Expression hierarchy, but we keep this for legacy nodes.
class NumberLiteralNode : public ValueNode {
public:
    Token value;
    explicit NumberLiteralNode(Token value) : value(std::move(value)) {}
    void accept(Visitor& visitor) override;
};


// Represents an element attribute
class AttributeNode : public BaseNode {
public:
    Token name;
    std::unique_ptr<ValueNode> value;
    AttributeNode(Token name, std::unique_ptr<ValueNode> value)
        : name(std::move(name)), value(std::move(value)) {}
    void accept(Visitor& visitor) override;
};


// Represents an HTML element
class ElementNode : public BaseNode {
public:
    Token tagName;
    std::vector<std::unique_ptr<AttributeNode>> attributes;
    std::unique_ptr<StyleBlockNode> styleBlock;
    std::vector<std::unique_ptr<BaseNode>> children;
    explicit ElementNode(Token tagName) : tagName(std::move(tagName)), styleBlock(nullptr) {}
    void accept(Visitor& visitor) override;
};


// Represents a text block
class TextNode : public BaseNode {
public:
    std::unique_ptr<ValueNode> value;
    explicit TextNode(std::unique_ptr<ValueNode> value) : value(std::move(value)) {}
    void accept(Visitor& visitor) override;
};


// Represents a generator comment
class CommentNode : public BaseNode {
public:
    Token comment;
    explicit CommentNode(Token comment) : comment(std::move(comment)) {}
    void accept(Visitor& visitor) override;
};


// The root node of the AST
class ProgramNode : public BaseNode {
public:
    std::vector<std::unique_ptr<BaseNode>> children;
    void accept(Visitor& visitor) override;
};


// --- Style-related Nodes ---
class StyleContentNode : public BaseNode {};

class StylePropertyNode : public StyleContentNode {
public:
    Token name;
    std::unique_ptr<ExpressionNode> value; // Changed from ValueNode
    StylePropertyNode(Token name, std::unique_ptr<ExpressionNode> value)
        : name(std::move(name)), value(std::move(value)) {}
    void accept(Visitor& visitor) override;
};

class StyleRuleNode : public StyleContentNode {
public:
    std::vector<Token> selector;
    std::vector<std::unique_ptr<StylePropertyNode>> properties;
    explicit StyleRuleNode(std::vector<Token> selector) : selector(std::move(selector)) {}
    void accept(Visitor& visitor) override;
};

class StyleBlockNode : public BaseNode {
public:
    std::vector<std::unique_ptr<StyleContentNode>> contents;
    void accept(Visitor& visitor) override;
};


// --- Template-related Nodes ---
class TemplateDefinitionNode : public BaseNode {
public:
    Token name;
};

class TemplateStyleNode : public TemplateDefinitionNode {
public:
    std::vector<std::unique_ptr<StylePropertyNode>> properties;
    void accept(Visitor& visitor) override;
};

class TemplateElementNode : public TemplateDefinitionNode {
public:
    std::vector<std::unique_ptr<BaseNode>> children;
    void accept(Visitor& visitor) override;
};

class UseStyleNode : public StyleContentNode {
public:
    Token name;
    explicit UseStyleNode(Token name) : name(std::move(name)) {}
    void accept(Visitor& visitor) override;
};

class UseElementNode : public BaseNode {
public:
    Token name;
    explicit UseElementNode(Token name) : name(std::move(name)) {}
    void accept(Visitor& visitor) override;
};


#endif // AST_H
