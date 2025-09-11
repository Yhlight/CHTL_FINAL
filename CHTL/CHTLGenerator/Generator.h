#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/OriginUsageNode.h"
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
    void visitOriginNode(const OriginNode* node, CHTLContext& context);
    void visitOriginUsageNode(const OriginUsageNode* node, CHTLContext& context);
};

#endif // CHTL_GENERATOR_H
