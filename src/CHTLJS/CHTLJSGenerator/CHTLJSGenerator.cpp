#include "CHTLJS/CHTLJSGenerator/CHTLJSGenerator.h"
#include "CHTLJS/CHTLJSNode/EnhancedSelectorNode.h"
#include "CHTLJS/CHTLJSNode/ListenNode.h"
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
            visitEnhancedSelector(static_cast<const EnhancedSelectorNode*>(node));
            break;
        case CHTLJSNodeType::Listen:
            visitListenNode(static_cast<const ListenNode*>(node));
            break;
        default:
            throw std::runtime_error("Unknown CHTL JS node type for generation.");
    }
}

void CHTLJSGenerator::visitEnhancedSelector(const EnhancedSelectorNode* node) {
    output_ += "document.querySelector(\"" + node->getSelector() + "\")";
}

void CHTLJSGenerator::visitListenNode(const ListenNode* node) {
    // First, generate the code for the object being listened to.
    visit(node->getObject());

    // Then, append the event listeners.
    for (const auto& event_pair : node->getEvents()) {
        output_ += ".addEventListener('" + event_pair.first + "', " + event_pair.second + ");";
    }
}


} // namespace CHTLJS
