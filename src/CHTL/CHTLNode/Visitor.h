#ifndef VISITOR_H
#define VISITOR_H

// Forward declarations of node types
class ElementNode;
class TextNode;
class CommentNode;
class OriginNode;

class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
    virtual void visit(CommentNode& node) = 0;
    virtual void visit(OriginNode& node) = 0;
};

#endif // VISITOR_H
