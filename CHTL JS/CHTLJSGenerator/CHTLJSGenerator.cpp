#include "CHTLJSGenerator.h"
#include "../CHTLJSNode/ListenNode.h"
#include "../CHTLJSNode/EnhancedSelectorNode.h"
#include "../CHTLJSNode/DelegateNode.h"
#include "../CHTLJSNode/RawJSNode.h"
#include "../CHTLJSNode/EventHandlerNode.h"
#include "../CHTLJSNode/AnimateNode.h"
#include <stdexcept>
#include <iostream>
#include <set>

namespace CHTL_JS {

    // Helper to generate the selector string from a ParsedSelector
    std::string getSelectorString(const ParsedSelector& selector) {
        if (selector.type == SelectorType::IndexedQuery && selector.index.has_value()) {
            return "document.querySelectorAll('" + selector.selector_string + "')[" + std::to_string(selector.index.value()) + "]";
        }
        return "document.querySelector('" + selector.selector_string + "')";
    }

    // Visitor Forward Declarations
    void visit(ListenNode* node, std::string& output);
    void visit(EnhancedSelectorNode* node, std::string& output);
    void visit(DelegateNode* node, std::string& output);
    void visit(RawJSNode* node, std::string& output);
    void visit(EventHandlerNode* node, std::string& output);
    void visit(AnimateNode* node, std::string& output);


    CHTLJSGenerator::CHTLJSGenerator() {}

    std::string CHTLJSGenerator::generate(const std::vector<std::unique_ptr<CHTLJSBaseNode>>& nodes) {
        m_output.clear();
        // A set to keep track of initialized parent listeners to avoid redundant setup code
        m_output += R"((function() {
    if (!window.__chtl_delegate_registry) {
        window.__chtl_delegate_registry = {};
    }
})();
)";
        for (const auto& node : nodes) {
            visit(node.get());
        }
        return m_output;
    }

    void CHTLJSGenerator::visit(CHTLJSBaseNode* node) {
        if (!node) return;

        if (auto listenNode = dynamic_cast<ListenNode*>(node)) {
            ::CHTL_JS::visit(listenNode, m_output);
        } else if (auto selectorNode = dynamic_cast<EnhancedSelectorNode*>(node)) {
            ::CHTL_JS::visit(selectorNode, m_output);
        } else if (auto delegateNode = dynamic_cast<DelegateNode*>(node)) {
            ::CHTL_JS::visit(delegateNode, m_output);
        } else if (auto rawJSNode = dynamic_cast<RawJSNode*>(node)) {
            ::CHTL_JS::visit(rawJSNode, m_output);
        } else if (auto eventHandlerNode = dynamic_cast<EventHandlerNode*>(node)) {
            ::CHTL_JS::visit(eventHandlerNode, m_output);
        } else if (auto animateNode = dynamic_cast<AnimateNode*>(node)) {
            ::CHTL_JS::visit(animateNode, m_output);
        }
    }

    // --- Visitor Implementations ---

    void visit(ListenNode* node, std::string& output) {
        std::string selector_str = getSelectorString(node->selector);
        for (const auto& pair : node->events) {
            const std::string& eventName = pair.first;
            const std::string& handler = pair.second;
            output += selector_str + ".addEventListener('" + eventName + "', " + handler + ");\n";
        }
    }

    void visit(EnhancedSelectorNode* node, std::string& output) {
        // Standalone selectors might just resolve to the DOM element in JS
        output += getSelectorString(node->parsed_selector) + ";\n";
    }

    void visit(RawJSNode* node, std::string& output) {
        output += node->content + "\n";
    }

    void visit(EventHandlerNode* node, std::string& output) {
        std::string selector_str = getSelectorString(node->selector);
        for (const auto& eventName : node->event_names) {
             output += selector_str + ".addEventListener('" + eventName + "', " + node->handler + ");\n";
        }
    }

    void visit(AnimateNode* node, std::string& output) {
        // Basic stub for AnimateNode
        output += "// AnimateNode generation not fully implemented yet.\n";
    }


    void visit(DelegateNode* node, std::string& output) {
        std::string parent_selector_id = node->parent_selector.selector_string; // Use selector string as a unique ID

        // Generate the code to add handlers to the registry
        for (const auto& event_pair : node->events) {
            const std::string& event_name = event_pair.first;
            const std::string& handler_code = event_pair.second;

            output += "if (!window.__chtl_delegate_registry['" + parent_selector_id + "']) {\n";
            output += "    window.__chtl_delegate_registry['" + parent_selector_id + "'] = { handlers: {}, element: document.querySelector('" + node->parent_selector.selector_string + "') };\n";
            output += "}\n";

            output += "if (!window.__chtl_delegate_registry['" + parent_selector_id + "'].handlers['" + event_name + "']) {\n";
            output += "    window.__chtl_delegate_registry['" + parent_selector_id + "'].handlers['" + event_name + "'] = {};\n";
            output += "    window.__chtl_delegate_registry['" + parent_selector_id + "'].element.addEventListener('" + event_name + "', function(e) {\n";
            output += "        const handlers = window.__chtl_delegate_registry['" + parent_selector_id + "'].handlers['" + event_name + "'];\n";
            output += "        for (const targetSelector in handlers) {\n";
            output += "            if (e.target.matches(targetSelector)) {\n";
            output += "                handlers[targetSelector].call(e.target, e);\n";
            output += "            }\n";
            output += "        }\n";
            output += "    });\n";
            output += "}\n";

            for (const auto& target_selector : node->target_selectors) {
                output += "window.__chtl_delegate_registry['" + parent_selector_id + "'].handlers['" + event_name + "']['" + target_selector.selector_string + "'] = " + handler_code + ";\n";
            }
        }
    }

} // namespace CHTL_JS
