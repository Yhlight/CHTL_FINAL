#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "../CHTLNode/AstVisitor.h"
#include <string>
#include <memory>

class Generator : public AstVisitor {
public:
    // The main entry point. Traverses the AST and returns the generated HTML.
    std::string generate(std::shared_ptr<class BaseNode> root);

    // Visitor methods for each node type
    void visit(class ElementNode& node) override;
    void visit(class TextNode& node) override;
    void visit(class CommentNode& node) override;
    void visit(class StyleNode& node) override;

private:
    std::string output;
    int indentLevel = 0;

    void indent();
};

#endif // CHTL_GENERATOR_H
