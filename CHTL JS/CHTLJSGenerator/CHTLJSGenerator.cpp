#include "CHTLJSGenerator.h"

std::string CHTLJSGenerator::generate(const std::vector<std::unique_ptr<CHTLJS_BaseNode>>& nodes, const std::string& parentSelector) {
    output.str("");
    for (const auto& node : nodes) {
        visit(node.get(), parentSelector);
    }
    return output.str();
}

void CHTLJSGenerator::visit(const CHTLJS_BaseNode* node, const std::string& parentSelector) {
    if (const auto* text = dynamic_cast<const CHTLJS_TextNode*>(node)) {
        visitTextNode(text);
    } else if (const auto* selector = dynamic_cast<const CHTLJS_SelectorNode*>(node)) {
        visitSelectorNode(selector, parentSelector);
    }
}

void CHTLJSGenerator::visitTextNode(const CHTLJS_TextNode* node) {
    output << node->content;
}

void CHTLJSGenerator::visitSelectorNode(const CHTLJS_SelectorNode* node, const std::string& parentSelector) {
    std::string selector = node->selector;
    // Trim whitespace from selector
    selector.erase(0, selector.find_first_not_of(" \t\n\r"));
    selector.erase(selector.find_last_not_of(" \t\n\r") + 1);

    if (selector == "&") {
        output << "document.querySelector('" << parentSelector << "')";
    } else {
        // This is a simplification. A real implementation would need more robust
        // selector parsing to handle all cases from CHTL.md.
        // For now, we just wrap it in querySelector.
        output << "document.querySelector('" << selector << "')";
    }
}
