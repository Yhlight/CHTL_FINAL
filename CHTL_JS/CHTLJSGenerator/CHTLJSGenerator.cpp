#include "CHTLJSGenerator.h"
#include "../CHTLJSNode/CHTLJSNode.h" // For ListenNode, SelectorNode
#include <iostream>

CHTLJSGenerator::CHTLJSGenerator(const std::vector<CHTLJSNodePtr>& ast) : ast(ast) {}

std::string CHTLJSGenerator::generate() {
    for (const auto& node : ast) {
        generateNode(node);
    }
    return output.str();
}

void CHTLJSGenerator::generateNode(const CHTLJSNodePtr& node) {
    if (auto* listen = dynamic_cast<ListenNode*>(node.get())) {
        visitListenNode(listen);
    }
    // Other node types will be added here
}

void CHTLJSGenerator::visitListenNode(ListenNode* node) {
    if (auto* selector = dynamic_cast<SelectorNode*>(node->target.get())) {
        // A real implementation would parse the body to get event types and handlers.
        // This is a major simplification.
        // Example: click: someClickHandler, mouseenter: someFunction
        // We'll just naively assume the first identifier is the event type for now.

        std::stringstream ss(node->body);
        std::string eventType, colon, handler;
        ss >> eventType >> colon >> handler;

        output << "document.querySelector('" << selector->selector_text << "').addEventListener('" << eventType << "', " << handler << ");\n";
    }
}

void CHTLJSGenerator::visitSelectorNode(SelectorNode* node) {
    // This is not called directly, it's used by other visitors.
}
