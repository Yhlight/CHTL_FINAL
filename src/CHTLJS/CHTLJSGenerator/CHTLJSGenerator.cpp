#include "CHTLJS/CHTLJSGenerator/CHTLJSGenerator.h"
#include "CHTLJS/CHTLJSNode/EnhancedSelectorNode.h"
#include <stdexcept>

namespace CHTLJS {

CHTLJSGenerator::CHTLJSGenerator() {}

std::string CHTLJSGenerator::generate(const CHTLJSNode& root) {
    visit(&root);
    return output_;
}

void CHTLJSGenerator::visit(const CHTLJSNode* node) {
    if (!node) return;
    switch (node->getType()) {
        case CHTLJSNodeType::EnhancedSelector:
            // In a real generator, we'd have a specific visit function,
            // but for this simple case, we can do it inline.
            output_ += "document.querySelector(\"" + static_cast<const EnhancedSelectorNode*>(node)->getSelector() + "\")";
            break;
        default:
            throw std::runtime_error("Unknown CHTL JS node type for generation.");
    }
}

} // namespace CHTLJS
