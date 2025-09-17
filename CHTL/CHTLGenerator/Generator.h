#pragma once

#include "../CHTLNode/Node.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/TemplateInstantiationNode.h"
#include "../CHTLContext.h"
#include <string>
#include <sstream>

namespace CHTL {

class Generator {
public:
    Generator(CHTLContext& context);
    std::string generate(Node* node);

private:
    CHTLContext& m_context;
    void visit(Node* node);
    void visitElementNode(ElementNode* node);
    void visitTextNode(TextNode* node);
    void visitTemplateInstantiationNode(TemplateInstantiationNode* node);

    std::stringstream m_output;
};

} // namespace CHTL
