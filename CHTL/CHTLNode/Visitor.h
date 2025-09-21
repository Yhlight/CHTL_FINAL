#ifndef VISITOR_H
#define VISITOR_H

namespace CHTL {

// Forward declarations of the concrete node types.
class ElementNode;
class TextNode;
class StyleNode;
class OriginNode;
class ScriptNode;
class OriginUsageNode;

// The Visitor interface declares a set of visiting methods that correspond
// to classes of the concrete nodes.
class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
    virtual void visit(StyleNode& node) = 0;
    virtual void visit(OriginNode& node) = 0;
    virtual void visit(ScriptNode& node) = 0;
    virtual void visit(OriginUsageNode& node) = 0;
};

} // namespace CHTL

#endif // VISITOR_H
