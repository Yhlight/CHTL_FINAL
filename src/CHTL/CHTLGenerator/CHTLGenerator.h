#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/RootNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/StyleBlockNode.h"
#include "../CHTLNode/OriginNode.h"
#include <string>
#include <sstream>
#include <memory>
#include "CssValueEvaluator.h" // For PropertyRegistry and ValueUnit

namespace CHTL {

class CHTLGenerator {
public:
    CHTLGenerator() = default;
    std::string generate(const RootNode& root);

private:
    void collectProperties(const Node* node);
    void visit(const Node* node);
    void visitElement(const ElementNode* node);
    void visitText(const TextNode* node);
    void visitComment(const CommentNode* node);
    void visitOrigin(const OriginNode* node);

    void indent();

    std::string renderCssValue(const std::vector<Token>& tokens);

    CssValueEvaluator::PropertyRegistry property_registry_;
    std::stringstream output_;
    std::stringstream global_styles_;
    int indentLevel_ = 0;
};

} // namespace CHTL

#endif // CHTL_GENERATOR_H
