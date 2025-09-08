#pragma once

#include <memory>
#include <vector>
#include <string>

// Forward declarations to avoid circular dependencies
class ElementNode;
class TextNode;

// Visitor pattern for traversing the AST
class AstVisitor {
public:
    virtual ~AstVisitor() = default;
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
};

class BaseNode {
public:
    virtual ~BaseNode() = default;
    virtual void accept(AstVisitor& visitor) = 0;
};
