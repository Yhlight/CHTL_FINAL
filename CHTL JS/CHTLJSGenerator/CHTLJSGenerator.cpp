#include "CHTLJSGenerator.h"
#include "../CHTLJSNode/ListenNode.h"
#include "../CHTLJSNode/EnhancedSelectorNode.h"
#include <stdexcept>
#include <iostream>

namespace CHTL_JS {

    // Forward declaration for the specific visit methods
    void visit(ListenNode* node, std::string& output);
    void visit(EnhancedSelectorNode* node, std::string& output);


    CHTLJSGenerator::CHTLJSGenerator() {}

    std::string CHTLJSGenerator::generate(CHTLJSBaseNode* root) {
        m_output.clear();
        if (root) {
            visit(root);
        }
        return m_output;
    }

    void CHTLJSGenerator::visit(CHTLJSBaseNode* node) {
        // This dispatcher will call the specific visit method based on the node's actual type.
        if (auto listenNode = dynamic_cast<ListenNode*>(node)) {
            ::CHTL_JS::visit(listenNode, m_output);
        } else if (auto selectorNode = dynamic_cast<EnhancedSelectorNode*>(node)) {
            ::CHTL_JS::visit(selectorNode, m_output);
        }
        // ... handle other node types

        // This is a simple implementation. A real implementation would need to
        // handle children traversal properly.
    }

    // Visitor implementations
    void visit(ListenNode* node, std::string& output) {
        // Generate the querySelector part from the selector_string
        std::string selector_str = "document.querySelector('" + node->selector.selector_string + "')";

        // Handle indexed selectors if needed
        if (node->selector.type == SelectorType::IndexedQuery && node->selector.index.has_value()) {
             selector_str = "document.querySelectorAll('" + node->selector.selector_string + "')[" + std::to_string(node->selector.index.value()) + "]";
        }


        // Generate the addEventListener part for each event
        for (const auto& pair : node->events) {
            const std::string& eventName = pair.first;
            const std::string& handler = pair.second;
            output += selector_str + ".addEventListener('" + eventName + "', " + handler + ");\n";
        }
    }

    void visit(EnhancedSelectorNode* node, std::string& output) {
        // This would generate code if a selector is used as a standalone expression.
        // For now, it's handled within the ListenNode visitor.
    }


} // namespace CHTL_JS
