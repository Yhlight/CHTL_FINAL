#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/RuleNode.h"
#include "../CHTLNode/DeclarationNode.h"
#include "GeneratorContext.h"
#include <string>
#include <sstream>

class Generator {
public:
    std::string generate(const BaseNode* root);

private:
    std::stringstream output;

    void visit(const BaseNode* node, GeneratorContext& context);
    void visitElementNode(const ElementNode* node, GeneratorContext& context);
    void visitTextNode(const TextNode* node, GeneratorContext& context);
    void visitCommentNode(const CommentNode* node, GeneratorContext& context);
    void visitStyleNode(const StyleNode* node, GeneratorContext& context, const ElementNode* parent);
    void visitRuleNode(const RuleNode* node, GeneratorContext& context, const ElementNode* parent);
    std::string visitDeclarationNode(const DeclarationNode* node, GeneratorContext& context);
};

#endif // CHTL_GENERATOR_H
