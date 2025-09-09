#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "CHTLNode/Visitor.h"
#include "CHTLNode/BaseNode.h"
#include <string>

class CHTLGenerator : public Visitor {
public:
    std::string generate(BaseNode& root);

    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(CommentNode& node) override;
    void visit(OriginNode& node) override;

private:
    std::string output;
    int indentLevel = 0;
    void doIndent();
};

#endif // CHTL_GENERATOR_H
