#pragma once

#include "../CHTLNode/Node.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include <string>
#include <sstream>

namespace CHTL {

class Generator {
public:
    Generator();
    std::string generate(Node* node);

private:
    void visit(Node* node);
    void visitElementNode(ElementNode* node);
    void visitTextNode(TextNode* node);
    // We will add more visit methods as we support more node types

    std::stringstream m_output;
};

} // namespace CHTL
