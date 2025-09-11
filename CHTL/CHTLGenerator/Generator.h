#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/RuleNode.h"
#include "../CHTLNode/DeclarationNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../CHTLContext.h"
#include <string>
#include <sstream>

class Generator {
public:
    std::string generate(const BaseNode* root, CHTLContext& context);

private:
    std::stringstream output;

    void visit(const BaseNode* node, CHTLContext& context);
    void visitElementNode(const ElementNode* node, CHTLContext& context);
    void visitTextNode(const TextNode* node, CHTLContext& context);
    void visitCommentNode(const CommentNode* node, CHTLContext& context);
    void visitStyleNode(const StyleNode* node, CHTLContext& context, const ElementNode* parent);
    void visitRuleNode(const RuleNode* node, CHTLContext& context, const ElementNode* parent);
    std::string visitDeclarationNode(const DeclarationNode* node, CHTLContext& context);
    std::string visitTemplateUsageNode(const TemplateUsageNode* node, CHTLContext& context, const ElementNode* parent);
};

#endif // CHTL_GENERATOR_H
