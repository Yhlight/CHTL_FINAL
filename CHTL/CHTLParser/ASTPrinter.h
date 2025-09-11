#pragma once

#include "CHTLNode/Node.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/CommentNode.h"
#include "CHTLNode/PropertyNode.h"
#include "CHTLNode/CustomNode.h"
#include "CHTLNode/ImportNode.h"
#include "CHTLNode/ConfigurationNode.h"
#include <iostream>

class ASTPrinter : public Visitor {
public:
    void print(const NodeList& nodes);

    void visit(ElementNode* node) override;
    void visit(TextNode* node) override;
    void visit(CommentNode* node) override;
    void visit(PropertyNode* node) override;
    void visit(StyleNode* node) override;
    void visit(StyleTemplateDefinitionNode* node) override;
    void visit(ElementTemplateDefinitionNode* node) override;
    void visit(VarTemplateDefinitionNode* node) override;
    void visit(StyleUsageNode* node) override;
    void visit(ElementUsageNode* node) override;
    void visit(CustomStyleDefinitionNode* node) override;
    void visit(CustomElementDefinitionNode* node) override;
    void visit(CustomVarDefinitionNode* node) override;
    void visit(DeleteNode* node) override;
    void visit(InsertNode* node) override;
    void visit(ImportNode* node) override;
    void visit(ConfigurationNode* node) override;

private:
    void indent();
    void dedent();
    void printIndent();

    int indent_level = 0;
};
