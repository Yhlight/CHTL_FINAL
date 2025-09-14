#include "CHTLJSGenerator.h"
#include <stdexcept>

namespace CHTLJS {

// =============================================================================
// Generator Public API
// =============================================================================

std::string CHTLJSGenerator::generate(RootNode& root) {
    output_.str(""); // Clear the stream
    output_.clear();
    visitRootNode(&root);
    return output_.str();
}

// =============================================================================
// Visitor Dispatch
// =============================================================================

void CHTLJSGenerator::visit(const CHTLJSNode* node) {
    if (!node) return;
    switch (node->getType()) {
        case CHTLJSNodeType::Root:
            visitRootNode(static_cast<const RootNode*>(node));
            break;
        case CHTLJSNodeType::EnhancedSelector:
            // This node doesn't generate code on its own, it's part of other expressions.
            // We call its visitor directly when we need the selector string.
            break;
        case CHTLJSNodeType::Listen:
            visitListenNode(static_cast<const ListenNode*>(node));
            break;
        case CHTLJSNodeType::Delegate:
            visitDelegateNode(static_cast<const DelegateNode*>(node));
            break;
        case CHTLJSNodeType::Animate:
            visitAnimateNode(static_cast<const AnimateNode*>(node));
            break;
        case CHTLJSNodeType::Router:
            visitRouterNode(static_cast<const RouterNode*>(node));
            break;
        case CHTLJSNodeType::Vir:
            visitVirNode(static_cast<const VirNode*>(node));
            break;
        case CHTLJSNodeType::Value:
            // Like EnhancedSelector, this is usually visited directly.
            break;
        default:
            throw std::runtime_error("Unknown CHTL JS node type for generation.");
    }
}

// =============================================================================
// Node-Specific Visitors
// =============================================================================

void CHTLJSGenerator::visitRootNode(const RootNode* node) {
    for (const auto& stmt : node->getStatements()) {
        visit(stmt.get());
    }
}

std::string CHTLJSGenerator::visitEnhancedSelectorNode(const EnhancedSelectorNode* node) {
    // This helper returns the JS code to get the element.
    // It's a bit of a simplification, as CHTL.md implies more complex selectors.
    std::string selector = node->getSelector();
    if (selector.rfind("->", 0) == 0 || selector.rfind(".", 0) == 0 || selector.rfind("#", 0) == 0) {
         return "document.querySelector('" + selector + "')";
    }
    // Bare identifier could be a tag name, class, or id.
    // A robust implementation would check context, but for now we default to a class selector.
    return "document.querySelector('." + selector + "')";
}

void CHTLJSGenerator::visitListenNode(const ListenNode* node) {
    auto object_selector = visitEnhancedSelectorNode(static_cast<const EnhancedSelectorNode*>(node->getObject()));
    for (const auto& event_pair : node->getEvents()) {
        output_ << object_selector << ".addEventListener('" << event_pair.first << "', " << event_pair.second << ");\n";
    }
}

void CHTLJSGenerator::visitDelegateNode(const DelegateNode* node) {
    // This is a complex feature. The generated code would involve a single listener
    // on the delegator with an if/switch statement inside checking `event.target.matches(...)`.
    // For now, we'll just output a comment.
    output_ << "/* Delegate Node for object: ";
    // Assuming the delegator is an EnhancedSelectorNode
    auto delegator_selector = visitEnhancedSelectorNode(static_cast<const EnhancedSelectorNode*>(node->getDelegator()));
    output_ << delegator_selector;
    output_ << " */\n";
}

void CHTLJSGenerator::visitAnimateNode(const AnimateNode* node) {
    // This is also very complex. The generated code uses the Web Animations API.
    // We will just output a comment placeholder.
    output_ << "/* Animate Node */\n";
}

void CHTLJSGenerator::visitRouterNode(const RouterNode* node) {
    // A full router implementation is a mini-framework.
    // We will just output a comment placeholder.
    output_ << "/* Router Node: mode='" << node->getMode() << "', root_path='" << node->getRootPath() << "' */\n";
}

void CHTLJSGenerator::visitVirNode(const VirNode* node) {
    // `vir` is a compile-time construct. It doesn't generate any runtime JavaScript.
    // Its purpose is to populate the parser's context. So, we do nothing here.
}

std::string CHTLJSGenerator::visitValueNode(const ValueNode* node) {
    if (node->is_string) {
        return "'" + node->value + "'";
    }
    return node->value;
}

} // namespace CHTLJS
