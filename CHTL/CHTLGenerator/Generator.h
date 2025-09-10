#pragma once

#include "CHTLNode/Node.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/CommentNode.h"
#include "CHTLNode/PropertyNode.h"
#include <string>
#include <sstream>

class Generator : public Visitor {
public:
    std::string generate(const NodeList& ast);

    void visit(ElementNode* node) override;
    void visit(TextNode* node) override;
    void visit(CommentNode* node) override;
    void visit(PropertyNode* node) override;

private:
    void indent();
    void dedent();
    void printIndent();

    std::stringstream output;
    int indent_level = 0;
    bool onNewLine = true;
};
