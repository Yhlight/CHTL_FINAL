#include "CHTLJSGenerator.h"
#include "../CHTLJSNode/ListenNode.h"
#include "../CHTLJSNode/EnhancedSelectorNode.h"
#include "../CHTLJSNode/AnimateNode.h"
#include <stdexcept>
#include <iostream>
#include <sstream>

namespace CHTL_JS {

    // Forward declaration for the specific visit methods
    void visit(ListenNode* node, std::string& output);
    void visit(EnhancedSelectorNode* node, std::string& output);
    void visit(AnimateNode* node, std::string& output);


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
        } else if (auto animateNode = dynamic_cast<AnimateNode*>(node)) {
            ::CHTL_JS::visit(animateNode, m_output);
        }
        // ... handle other node types
    }

    // Helper to convert property map to JS object string
    std::string map_to_js_object(const std::map<std::string, std::string>& props) {
        std::stringstream ss;
        ss << "{";
        for (auto it = props.begin(); it != props.end(); ++it) {
            ss << "'" << it->first << "': '" << it->second << "'";
            if (std::next(it) != props.end()) {
                ss << ", ";
            }
        }
        ss << "}";
        return ss.str();
    }

    void visit(AnimateNode* node, std::string& output) {
        output += "(() => {\n";

        // 1. Get targets
        output += "  const targets = [];\n";
        for (const auto& target : node->targets) {
            if (target.type == SelectorType::IndexedQuery && target.index.has_value()) {
                output += "  targets.push(document.querySelectorAll('" + target.selector_string + "')[" + std::to_string(target.index.value()) + "]);\n";
            } else {
                output += "  targets.push(...document.querySelectorAll('" + target.selector_string + "'));\n";
            }
        }

        // 2. Get animation parameters
        output += "  const duration = " + (node->duration.has_value() ? std::to_string(node->duration.value()) : "1000") + ";\n";
        output += "  const beginState = " + map_to_js_object(node->begin_state) + ";\n";
        output += "  const endState = " + map_to_js_object(node->end_state) + ";\n";

        // 3. Animation loop
        output += "  let startTime = null;\n";
        output += "  function step(timestamp) {\n";
        output += "    if (!startTime) startTime = timestamp;\n";
        output += "    const progress = Math.min((timestamp - startTime) / duration, 1);\n";
        output += "    targets.forEach(target => {\n";
        output += "      for (const prop in endState) {\n";
        // This is a simplified linear interpolation (lerp)
        output += "        const start = parseFloat(beginState[prop] || 0);\n";
        output += "        const end = parseFloat(endState[prop] || 0);\n";
        output += "        const unit = isNaN(endState[prop]) ? endState[prop].replace(/[\\d\\.-]/g, '') : 'px';\n";
        output += "        target.style[prop] = (start + (end - start) * progress) + unit;\n";
        output += "      }\n";
        output += "    });\n";
        output += "    if (progress < 1) {\n";
        output += "      requestAnimationFrame(step);\n";
    output += "    } else {\n";
    if (node->callback.has_value()) {
        output += "      const callback = " + node->callback.value() + ";\n";
        output += "      if (callback) callback();\n";
    }
        output += "    }\n";
        output += "  }\n";

        // 4. Start animation
        output += "  requestAnimationFrame(step);\n";
        output += "})();\n";
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
