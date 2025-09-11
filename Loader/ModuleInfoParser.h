#pragma once

#include "CHTLNode/Node.h"
#include "CHTLNode/ModuleNode.h"
#include "CHTLNode/ConfigurationNode.h"
#include "CHTLNode/PropertyNode.h"
#include "CHTLNode/ExprNode.h"
#include <memory>

class ModuleInfoParser : public Visitor {
public:
    ModuleInfo parse(NodeList& ast);

    void visit(ConfigurationNode* node) override;
    void visit(ExportNode* node) override;

    // Ignore all other nodes
    void visit(ElementNode* n) override{} void visit(TextNode* n) override{}
    void visit(CommentNode* n) override{} void visit(PropertyNode* n) override{}
    void visit(StyleNode* n) override{} void visit(ScriptNode* n) override{}
    void visit(StyleTemplateDefinitionNode* n) override{} void visit(ElementTemplateDefinitionNode* n) override{}
    void visit(VarTemplateDefinitionNode* n) override{} void visit(StyleUsageNode* n) override{}
    void visit(ElementUsageNode* n) override{} void visit(CustomStyleDefinitionNode* n) override{}
    void visit(CustomElementDefinitionNode* n) override{} void visit(CustomVarDefinitionNode* n) override{}
    void visit(DeleteNode* n) override{} void visit(InsertNode* n) override{}
    void visit(ImportNode* n) override{} void visit(NamespaceNode* n) override{}
    void visit(UseNode* n) override{} void visit(ConstraintNode* n) override{}

private:
    std::unique_ptr<ModuleInfo> module_info;
};
