#ifndef AST_PRINTER_H
#define AST_PRINTER_H

#include "../../CHTL/CHTLNode/Visitor.h"
#include "../../CHTL/CHTLNode/BaseNode.h"
#include <string>

namespace CHTL {

// An implementation of the Visitor pattern that prints a human-readable
// representation of the AST to the console.
class ASTPrinter : public Visitor {
public:
    void print(BaseNode* node);

    // Visitor methods for each concrete node type.
    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(StyleNode& node) override;
    void visit(OriginNode& node) override;
    void visit(CssRuleNode& node) override;
    void visit(DocumentNode& node) override;

private:
    std::string indentString();
    int indent = 0;
};

} // namespace CHTL

#endif // AST_PRINTER_H
