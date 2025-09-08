#pragma once

#include <memory>
#include <vector>
#include <string>

// Forward declarations to avoid circular dependencies
class ElementNode;
class TextNode;
class StyleNode;
class TemplateUsageNode;
class OriginNode;

// Visitor pattern for traversing the AST
class AstVisitor {
public:
    virtual ~AstVisitor() = default;
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
    virtual void visit(StyleNode& node) = 0;
    virtual void visit(TemplateUsageNode& node) = 0;
    virtual void visit(OriginNode& node) = 0;
};

class BaseNode {
public:
    virtual ~BaseNode() = default;
    virtual void accept(AstVisitor& visitor) = 0;
};
