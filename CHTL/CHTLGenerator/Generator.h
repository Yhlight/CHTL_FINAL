#pragma once

#include "CHTLNode/Node.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/CommentNode.h"
#include "CHTLNode/PropertyNode.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/ScriptNode.h"
#include "CHTLNode/TemplateNode.h"
#include "CHTLNode/CustomNode.h"
#include "CHTLNode/ImportNode.h"
#include "CHTLNode/ConfigurationNode.h"
#include "CHTLNode/NamespaceNode.h"
#include "CHTLNode/UseNode.h"
#include "CHTLNode/ConstraintNode.h"
#include "CHTLEvaluator/ExprEvaluator.h"
#include <string>
#include <sstream>
#include <map>

class Generator : public Visitor {
public:
    std::string generate(NodeList& ast);

    void visit(ElementNode* node) override;
    void visit(TextNode* node) override;
    void visit(CommentNode* node) override;
    void visit(PropertyNode* node) override;
    void visit(ExportNode* node) override;
    void visit(StyleNode* node) override;
    void visit(ScriptNode* node) override;
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
    void visit(NamespaceNode* node) override;
    void visit(UseNode* node) override;
    void visit(ConstraintNode* node) override;

private:
    void indent();
    void dedent();
    void printIndent();

    // Namespace-aware symbol tables
    using StyleTemplateMap = std::map<std::string, StyleTemplateDefinitionNode*>;
    using ElementTemplateMap = std::map<std::string, ElementTemplateDefinitionNode*>;
    using VarTemplateMap = std::map<std::string, VarTemplateDefinitionNode*>;

    std::map<std::string, StyleTemplateMap> styleTemplates;
    std::map<std::string, ElementTemplateMap> elementTemplates;
    std::map<std::string, VarTemplateMap> varTemplates;

    Node* ast_root = nullptr;
    std::stringstream output;
    int indent_level = 0;
    bool onNewLine = true;
};
