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

        // This is still a major simplification.
        // A real implementation would parse the key-value pairs properly.
        std::string body = node->body;
        std::string eventType = body.substr(0, body.find(":"));
        std::string handler = body.substr(body.find(":") + 1);

        // Trim whitespace
        handler.erase(0, handler.find_first_not_of(" \n\r\t"));
        handler.erase(handler.find_last_not_of(" \n\r\t") + 1);
        eventType.erase(eventType.find_last_not_of(" \n\r\t") + 1);


        output << "document.querySelector('" << selector->selector_text << "').addEventListener('" << eventType << "', " << handler << ");\n";
    }
}

void CHTLJSGenerator::visitSelectorNode(SelectorNode* node) {
    // This is not called directly, it's used by other visitors.
}
