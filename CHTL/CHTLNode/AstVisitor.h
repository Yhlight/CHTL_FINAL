#ifndef CHTL_AST_VISITOR_H
#define CHTL_AST_VISITOR_H

// Forward declare the node types to avoid circular dependencies
class ElementNode;
class TextNode;
class CommentNode;
class StyleNode;
class StyleTemplateNode;

class AstVisitor {
public:
    virtual ~AstVisitor() = default;

    // A visit method for each concrete node type
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
    virtual void visit(CommentNode& node) = 0;
    virtual void visit(StyleNode& node) = 0;
    virtual void visit(StyleTemplateNode& node) = 0;
};

#endif // CHTL_AST_VISITOR_H
