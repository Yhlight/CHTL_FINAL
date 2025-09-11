#pragma once

#include "CHTLNode/Node.h"
#include "CHTLNode/ImportNode.h"
#include <vector>

// This visitor finds all [Import] blocks in an AST.
class ImportVisitor : public Visitor {
public:
    ImportVisitor() = default;

    void run(const NodeList& ast);
    const std::vector<ImportNode*>& getImports() const { return import_nodes; }

    void visit(ImportNode* node) override;

    // We need to traverse the whole tree to find imports
    void visit(ElementNode* node) override;
    void visit(StyleNode* node) override;

    // Empty implementations for other node types
    void visit(TextNode* node) override {}
    void visit(CommentNode* node) override {}
    void visit(PropertyNode* node) override {}
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
    void visit(ConfigurationNode* node) override {}


private:
    std::vector<ImportNode*> import_nodes;
};
