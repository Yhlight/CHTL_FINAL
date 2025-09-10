#pragma once

#include "CHTLNode/Node.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/CommentNode.h"
#include "CHTLNode/PropertyNode.h"
#include <string>
#include <sstream>
#include <map>

class Generator : public Visitor {
public:
    std::string generate(const NodeList& ast);

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

private:
    void indent();
    void dedent();
    void printIndent();

    // Symbol tables for templates
    std::map<std::string, StyleTemplateDefinitionNode*> styleTemplates;
    std::map<std::string, ElementTemplateDefinitionNode*> elementTemplates;
    std::map<std::string, VarTemplateDefinitionNode*> varTemplates;

    std::stringstream output;
    int indent_level = 0;
    bool onNewLine = true;
};
