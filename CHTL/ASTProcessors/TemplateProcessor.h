#ifndef CHTL_TEMPLATE_PROCESSOR_H
#define CHTL_TEMPLATE_PROCESSOR_H

#include "../CHTLNode/AstVisitor.h"
#include <memory>

class TemplateProcessor : public AstVisitor {
public:
    // The main entry point. Traverses and modifies the AST.
    void process(std::shared_ptr<class BaseNode> root, class CHTLContext& context);

    // Visitor methods
    void visit(class ElementNode& node) override;
    void visit(class TextNode& node) override;
    void visit(class CommentNode& node) override;
    void visit(class StyleNode& node) override;
    void visit(class StyleTemplateNode& node) override;
    void visit(class ElementTemplateNode& node) override;
    void visit(class ElementTemplateUsageNode& node) override;

private:
    class CHTLContext* context = nullptr;
};

#endif // CHTL_TEMPLATE_PROCESSOR_H
