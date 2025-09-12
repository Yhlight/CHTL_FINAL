#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/RootNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include <string>
#include <sstream>
#include <memory>

namespace CHTL {

class CHTLGenerator {
public:
    CHTLGenerator() = default;
    std::string generate(const RootNode& root);

private:
    void visit(const Node* node);
    void visitElement(const ElementNode* node);
    void visitText(const TextNode* node);
    void visitComment(const CommentNode* node);

    void indent();

    std::stringstream output_;
    int indentLevel_ = 0;
};

} // namespace CHTL

#endif // CHTL_GENERATOR_H
