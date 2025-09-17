#ifndef AST_PRINTER_H
#define AST_PRINTER_H

#include "CHTLNode/AST.h"
#include <string>
#include <vector>
#include <sstream>

class AstPrinter : public Visitor {
public:
    // Main entry point to print an AST from any node.
    std::string print(BaseNode& node);

    // Visitor methods for each node type.
    void visit(ProgramNode& node) override;
    void visit(ElementNode& node) override;
    void visit(AttributeNode& node) override;
    void visit(TextNode& node) override;
    void visit(CommentNode& node) override;
    void visit(StringLiteralNode& node) override;
    void visit(UnquotedLiteralNode& node) override;
    void visit(NumberLiteralNode& node) override;
    void visit(StyleBlockNode& node) override;
    void visit(StylePropertyNode& node) override;
    void visit(StyleRuleNode& node) override;
    void visit(TemplateStyleNode& node) override;
    void visit(TemplateElementNode& node) override;
    void visit(UseStyleNode& node) override;
    void visit(UseElementNode& node) override;

private:
    std::stringstream buffer;

    // Helper to create the s-expression format.
    template<typename... Args>
    void parenthesize(const std::string& name, Args... args);
};

#endif // AST_PRINTER_H
