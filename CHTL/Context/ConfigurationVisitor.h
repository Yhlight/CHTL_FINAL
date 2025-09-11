#pragma once

#include "CHTLNode/Node.h"
#include "CHTLNode/ConfigurationNode.h"
#include "ConfigurationContext.h"

// This visitor finds all [Configuration] blocks and populates the context.
class ConfigurationVisitor : public Visitor {
public:
    ConfigurationVisitor(ConfigurationContext& context);

    void run(const NodeList& ast);

    // We only care about ConfigurationNode in this visitor.
    void visit(ConfigurationNode* node) override;

    // Empty implementations for all other node types
    void visit(ElementNode* node) override {}
    void visit(TextNode* node) override {}
    void visit(CommentNode* node) override {}
    void visit(PropertyNode* node) override {}
    void visit(StyleNode* node) override {}
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


private:
    ConfigurationContext& context;
};
