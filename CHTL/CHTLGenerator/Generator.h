#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include <string>
#include <sstream>

class Generator {
public:
    std::string generate(const BaseNode* root);

private:
    std::stringstream output;

    void visit(const BaseNode* node);
    void visitElementNode(const ElementNode* node);
    void visitTextNode(const TextNode* node);
    void visitCommentNode(const CommentNode* node);
};

#endif // CHTL_GENERATOR_H
