#pragma once

#include "CHTLNode/Node.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/CommentNode.h"
#include "CHTLNode/PropertyNode.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/TemplateNode.h"
#include "CHTLNode/CustomNode.h"
#include "CHTLNode/ExprNode.h"

// This visitor clones a node or a list of nodes.
class ASTCloner : public Visitor {
public:
    NodePtr clone(Node* node);
    NodeList clone(const NodeList& nodes);

    void visit(ElementNode* node) override;
    void visit(TextNode* node) override;
    void visit(CommentNode* node) override;
    void visit(PropertyNode* node) override;
    void visit(StyleNode* node) override;
    // We don't need to clone template definitions/usages in this context,
    // but we need to provide empty implementations for the visitor interface.
    void visit(StyleTemplateDefinitionNode* node) override {}
    void visit(ElementTemplateDefinitionNode* node) override {}
    void visit(VarTemplateDefinitionNode* node) override {}
    void visit(StyleUsageNode* node) override {}
    void visit(ElementUsageNode* node) override {}
    void visit(CustomStyleDefinitionNode* node) override {}
    void visit(CustomElementDefinitionNode* node) override {}
    void visit(CustomVarDefinitionNode* node) override {}
    void visit(DeleteNode* node) override {}
    void visit(InsertNode* node) override {}
    void visit(ImportNode* node) override {}
    void visit(ConfigurationNode* node) override {}

private:
    // Helper for cloning expression trees
    ExprNodePtr cloneExpr(ExprNode* expr);

    NodePtr cloned_node;
};
