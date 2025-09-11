#pragma once

// Forward declare all the node types
namespace CHTL {
    class ElementNode;
    class TextNode;
    class AttributeNode;
    class CommentNode;
    class StyleNode;
    class TemplateDefinitionNode;
    class TemplateUsageNode;
    class StyleRuleNode;
    class CustomDefinitionNode;
    class ImportNode;
    class NamespaceNode;
}

namespace CHTL {

class AstVisitor {
public:
    virtual ~AstVisitor() = default;
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
    virtual void visit(AttributeNode& node) = 0;
    virtual void visit(CommentNode& node) = 0;
    virtual void visit(StyleNode& node) = 0;
    virtual void visit(TemplateDefinitionNode& node) = 0;
    virtual void visit(TemplateUsageNode& node) = 0;
    virtual void visit(StyleRuleNode& node) = 0;
    virtual void visit(CustomDefinitionNode& node) = 0;
    virtual void visit(ImportNode& node) = 0;
    virtual void visit(NamespaceNode& node) = 0;
};

} // namespace CHTL
