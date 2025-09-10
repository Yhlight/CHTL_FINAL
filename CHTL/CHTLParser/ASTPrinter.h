#pragma once

#include "CHTLNode/Node.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/CommentNode.h"
#include "CHTLNode/PropertyNode.h"
#include <iostream>

class ASTPrinter : public Visitor {
public:
    void print(const NodeList& nodes);

    void visit(ElementNode* node) override;
    void visit(TextNode* node) override;
    void visit(CommentNode* node) override;
    void visit(PropertyNode* node) override;

private:
    void indent();
    void dedent();
    void printIndent();

    int indent_level = 0;
};
