#ifndef CHTL_STYLE_EVALUATOR_H
#define CHTL_STYLE_EVALUATOR_H

#include "../CHTLNode/AstVisitor.h"
#include <memory>

class StyleEvaluator : public AstVisitor {
public:
    // The main entry point. Traverses and modifies the AST.
    void process(std::shared_ptr<class BaseNode> root);

    // Visitor methods
    void visit(class ElementNode& node) override;
    void visit(class TextNode& node) override;
    void visit(class CommentNode& node) override;
    void visit(class StyleNode& node) override;
    void visit(class StyleTemplateNode& node) override;

private:
    std::shared_ptr<BaseNode> astRoot;
};

#endif // CHTL_STYLE_EVALUATOR_H
