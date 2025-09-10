#pragma once

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/NamespaceNode.h"
#include "../CHTLNode/TemplateNode.h"
#include "../CHTLNode/CustomNode.h"
#include "../CHTLNode/ExprNode.h"
#include <string>
#include <sstream>
#include <map>

class Generator {
public:
    Generator(const NodeList& ast);
    std::string generate();

private:
    const NodeList& ast;
    std::stringstream output;
    std::map<std::string, ElementNode*> symbol_map;
    std::map<std::string, TemplateNode*> template_map;
    std::map<std::string, CustomNode*> custom_map;

    void buildSymbolMap(const AstNodePtr& node);
    void buildTemplateMap(const AstNodePtr& node);
    void buildCustomMap(const AstNodePtr& node);
    void generateNode(const AstNodePtr& node, ElementNode* context = nullptr);

    // Node visitors
    void visitElementNode(ElementNode* node);
    void visitTextNode(TextNode* node);
    void visitCommentNode(CommentNode* node);
    void visitOriginNode(OriginNode* node);
    void visitNamespaceNode(NamespaceNode* node);
    void visitTemplateUsageNode(TemplateUsageNode* node, ElementNode* context);
    void applyStyleTemplate(ElementNode* context, TemplateNode* tmpl);
    void applyCustomStyle(ElementNode* context, CustomNode* custom, TemplateUsageNode* usage);

    // Expression generator
    std::string generateExpression(const std::unique_ptr<ExprNode>& expr, ElementNode* context);
};
