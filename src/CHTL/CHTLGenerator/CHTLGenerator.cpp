#include "CHTLGenerator.h"
#include "CHTL/CHTLNode/ElementNode.h"

namespace CHTL {

std::string CHTLGenerator::generate(BaseNode* root) {
    if (root) {
        root->accept(*this);
    }
    return output.str();
}

void CHTLGenerator::visit(ElementNode& node) {
    output << "<" << node.getTagName() << ">";

    // Recursively visit children
    for (const auto& child : node.getChildren()) {
        child->accept(*this);
    }

    output << "</" << node.getTagName() << ">";
}

// Other visit methods are empty in the header for now.

} // namespace CHTL
