#ifndef CHTL_NODEVISITOR_H
#define CHTL_NODEVISITOR_H

namespace CHTL {

// Forward declare all node types for the visitor interface
class ElementNode;
class TextNode;
class CommentNode;
class TemplateNode;
class CustomNode;
class StyleNode;
class ScriptNode;
class OriginNode;
class ImportNode;
class ConfigNode;
class NamespaceNode;
class OperatorNode;

class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;

    virtual void visit(ElementNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
    virtual void visit(CommentNode& node) = 0;
    virtual void visit(TemplateNode& node) = 0;
    virtual void visit(CustomNode& node) = 0;
    virtual void visit(StyleNode& node) = 0;
    virtual void visit(ScriptNode& node) = 0;
    virtual void visit(OriginNode& node) = 0;
    virtual void visit(ImportNode& node) = 0;
    virtual void visit(ConfigNode& node) = 0;
    virtual void visit(NamespaceNode& node) = 0;
    virtual void visit(OperatorNode& node) = 0;
};

} // namespace CHTL

#endif // CHTL_NODEVISITOR_H
