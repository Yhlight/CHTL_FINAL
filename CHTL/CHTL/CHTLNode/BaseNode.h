#pragma once

#include <memory>
#include <vector>

// Forward declarations to resolve circular dependencies
class ElementNode;
class TextNode;
class AttributeNode;
class RootNode;
class StyleNode;
class PropertyNode;
class RuleNode;
class TemplateDefinitionNode;
class TemplateUsageNode;

// Visitor pattern for traversing the AST
class AstVisitor {
public:
    virtual ~AstVisitor() = default;
    virtual void visit(RootNode& node) = 0;
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(AttributeNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
    virtual void visit(StyleNode& node) = 0;
    virtual void visit(PropertyNode& node) = 0;
    virtual void visit(RuleNode& node) = 0;
    virtual void visit(TemplateDefinitionNode& node) = 0;
    virtual void visit(TemplateUsageNode& node) = 0;
};

class BaseNode {
public:
    virtual ~BaseNode() = default;
    virtual void accept(AstVisitor& visitor) = 0;
};
