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
struct SelectorNode;
struct TemplateDefinitionNode;
struct TemplateUsageNode;
// Expression Nodes
struct ExpressionNode;
struct LiteralNode;
struct BinaryOpNode;

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
    virtual void visit(SelectorNode& node) = 0;
    virtual void visit(TemplateDefinitionNode& node) = 0;
    virtual void visit(TemplateUsageNode& node) = 0;
};

// --- Expression Nodes ---

struct ExpressionNode : public Node {
    void accept(INodeVisitor& visitor) override {
        // This node is not directly visited; it's handled by its parent.
    }
};

struct LiteralNode : public ExpressionNode {
    Token token;
    explicit LiteralNode(const Token& token) : token(token) {}
};

struct BinaryOpNode : public ExpressionNode {
    std::unique_ptr<ExpressionNode> left;
    Token op;
    std::unique_ptr<ExpressionNode> right;
    BinaryOpNode(std::unique_ptr<ExpressionNode> left, Token op, std::unique_ptr<ExpressionNode> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}
};


// --- Statement and Declaration Nodes ---

struct TemplateDefinitionNode : public Node {
    Token template_type; // @Style, @Element, @Var
    Token name;
    std::vector<std::unique_ptr<Node>> children;
    void accept(INodeVisitor& visitor) override { visitor.visit(*this); }
};

struct TemplateUsageNode : public Node {
    Token template_type; // @Style, @Element, @Var
    Token name;
    void accept(INodeVisitor& visitor) override { visitor.visit(*this); }
};

struct AttributeNode : public Node {
    std::string key;
    std::unique_ptr<ExpressionNode> value;
    AttributeNode(const std::string& key, std::unique_ptr<ExpressionNode> value)
        : key(key), value(std::move(value)) {}
    void accept(INodeVisitor& visitor) override { visitor.visit(*this); }
};

struct StyleNode : public Node {
    std::vector<std::unique_ptr<Node>> children;
    void accept(INodeVisitor& visitor) override { visitor.visit(*this); }
};

struct SelectorNode : public Node {
    std::string selector;
    std::vector<std::unique_ptr<AttributeNode>> properties;
    explicit SelectorNode(const std::string& selector) : selector(selector) {}
    void accept(INodeVisitor& visitor) override { visitor.visit(*this); }
};

struct TextNode : public Node {
    std::unique_ptr<ExpressionNode> content;
    explicit TextNode(std::unique_ptr<ExpressionNode> content) : content(std::move(content)) {}
    void accept(INodeVisitor& visitor) override { visitor.visit(*this); }
};

struct ElementNode : public Node {
    std::string tagName;
    std::vector<std::unique_ptr<Node>> children;
    explicit ElementNode(const std::string& tagName) : tagName(tagName) {}
    void accept(INodeVisitor& visitor) override { visitor.visit(*this); }
};

struct ProgramNode : public Node {
    std::vector<std::unique_ptr<Node>> children;
    void accept(INodeVisitor& visitor) override { visitor.visit(*this); }
};
