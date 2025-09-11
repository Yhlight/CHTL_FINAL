#pragma once

#include "CHTLNode/Node.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/ConstraintNode.h"
#include <vector>
#include <string>
#include <stdexcept>
#include <set>

class SemanticAnalyzer : public Visitor {
public:
    explicit SemanticAnalyzer(NodeList& ast);

    void analyze();

    class SemanticError : public std::runtime_error {
    public:
        SemanticError(const std::string& message) : std::runtime_error(message) {}
    };

    // We only need to implement the visitors for nodes that can contain other nodes
    // or are relevant to semantic checks.
    void visit(ElementNode* node) override;
    void visit(NamespaceNode* node) override;

    // Empty implementations for leaf nodes or irrelevant nodes
    void visit(TextNode* n) override{} void visit(CommentNode* n) override{}
    void visit(PropertyNode* n) override{} void visit(StyleNode* n) override{}
    void visit(ScriptNode* n) override{} void visit(StyleTemplateDefinitionNode* n) override{}
    void visit(ElementTemplateDefinitionNode* n) override{} void visit(VarTemplateDefinitionNode* n) override{}
    void visit(StyleUsageNode* n) override{} void visit(ElementUsageNode* n) override{}
    void visit(CustomStyleDefinitionNode* n) override{} void visit(CustomElementDefinitionNode* n) override{}
    void visit(CustomVarDefinitionNode* n) override{} void visit(DeleteNode* n) override{}
    void visit(InsertNode* n) override{} void visit(ImportNode* n) override{}
    void visit(ConfigurationNode* n) override{} void visit(UseNode* n) override{}
    void visit(ConstraintNode* n) override{}

private:
    NodeList& ast;
};
