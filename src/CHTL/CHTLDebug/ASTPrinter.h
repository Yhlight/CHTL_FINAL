#ifndef AST_PRINTER_H
#define AST_PRINTER_H

#include "CHTLNode/Visitor.h"
#include "CHTLNode/BaseNode.h" // Include BaseNode definition
#include <string>

class ASTPrinter : public Visitor {
public:
    std::string print(BaseNode& node);
    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(CommentNode& node) override;

private:
    std::string result;
    int indentLevel = 0;
    void indent();
};

#endif // AST_PRINTER_H
