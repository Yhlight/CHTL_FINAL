#pragma once

#include <memory>
#include <string>
#include <vector>

// Forward declare all node types and the visitor
class Node;
class ElementNode;
class TextNode;
class CommentNode;
class PropertyNode;

// Visitor interface for the AST
class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visit(ElementNode* node) = 0;
    virtual void visit(TextNode* node) = 0;
    virtual void visit(CommentNode* node) = 0;
    virtual void visit(PropertyNode* node) = 0;
};

// Base class for all AST nodes
class Node {
public:
    virtual ~Node() = default;
    virtual void accept(Visitor& visitor) = 0;
};

// Use unique_ptr for memory management of nodes
using NodePtr = std::unique_ptr<Node>;
using NodeList = std::vector<NodePtr>;
