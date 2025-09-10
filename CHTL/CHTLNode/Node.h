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
class StyleNode;
class StyleTemplateDefinitionNode;
class ElementTemplateDefinitionNode;
class VarTemplateDefinitionNode;
class StyleUsageNode;
class ElementUsageNode;
class CustomStyleDefinitionNode;
class CustomElementDefinitionNode;
class CustomVarDefinitionNode;
class DeleteNode;
class InsertNode;

// Visitor interface for the AST
class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visit(ElementNode* node) = 0;
    virtual void visit(TextNode* node) = 0;
    virtual void visit(CommentNode* node) = 0;
    virtual void visit(PropertyNode* node) = 0;
    virtual void visit(StyleNode* node) = 0;
    virtual void visit(StyleTemplateDefinitionNode* node) = 0;
    virtual void visit(ElementTemplateDefinitionNode* node) = 0;
    virtual void visit(VarTemplateDefinitionNode* node) = 0;
    virtual void visit(StyleUsageNode* node) = 0;
    virtual void visit(ElementUsageNode* node) = 0;
    virtual void visit(CustomStyleDefinitionNode* node) = 0;
    virtual void visit(CustomElementDefinitionNode* node) = 0;
    virtual void visit(CustomVarDefinitionNode* node) = 0;
    virtual void visit(DeleteNode* node) = 0;
    virtual void visit(InsertNode* node) = 0;
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
