#ifndef CHTL_STYLE_PROCESSOR_H
#define CHTL_STYLE_PROCESSOR_H

#include "../CHTLNode/AstVisitor.h"
#include <memory>

class StyleProcessor : public AstVisitor {
public:
    // The main entry point. Traverses and modifies the AST.
    void process(std::shared_ptr<class BaseNode> root);

    // Visitor methods for each node type
    void visit(class ElementNode& node) override;
    void visit(class TextNode& node) override;
    void visit(class CommentNode& node) override;
    void visit(class StyleNode& node) override;

private:
    // A pointer to the current element being processed, so that
    // when we visit a StyleNode, we know its parent element.
    ElementNode* currentElement = nullptr;
};

#endif // CHTL_STYLE_PROCESSOR_H
