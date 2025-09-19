#include "CHTLJSGenerator.h"
#include "../CHTLJSNode/AnimateNode.h"
#include "../CHTLJSNode/DelegateNode.h"
#include "../CHTLJSNode/EnhancedSelectorNode.h"
#include "../CHTLJSNode/EventHandlerNode.h"
#include "../CHTLJSNode/ListenNode.h"
#include "../CHTLJSNode/RawJSNode.h"

namespace CHTL_JS {

CHTLJSGenerator::CHTLJSGenerator() {}

std::string CHTLJSGenerator::generate(const std::vector<std::unique_ptr<CHTLJSBaseNode>>& nodes) {
    js_output.str("");
    for (const auto& node : nodes) {
        if (node) {
            node->accept(*this);
        }
    }
    return js_output.str();
}

void CHTLJSGenerator::visit(AnimateNode& node) {
    js_output << "{\n";
    js_output << "  const targets = [";
    for (size_t i = 0; i < node.targets.size(); ++i) {
        js_output << "document.querySelector('" << node.targets[i].selector_string << "')";
        if (i < node.targets.size() - 1) js_output << ", ";
    }
    js_output << "];\n";
    js_output << "  const duration = " << node.duration.value_or(1000) << ";\n";
    js_output << "  let startTime = null;\n";
    js_output << "  function step(timestamp) {\n";
    js_output << "    if (!startTime) startTime = timestamp;\n";
    js_output << "    const progress = Math.min((timestamp - startTime) / duration, 1);\n";
    js_output << "    targets.forEach(target => {\n";
    js_output << "    });\n";
    js_output << "    if (progress < 1) {\n";
    js_output << "      requestAnimationFrame(step);\n";
    js_output << "    }\n";
    js_output << "  }\n";
    js_output << "  requestAnimationFrame(step);\n";
    js_output << "}\n";
}

void CHTLJSGenerator::visit(DelegateNode& node) {
    js_output << "document.querySelector('" << node.parent_selector.selector_string << "').addEventListener('click', (event) => {\n";
    for (const auto& target : node.target_selectors) {
         js_output << "  if (event.target.matches('" << target.selector_string << "')) {\n";
         for (const auto& event : node.events) {
             js_output << "    (" << event.second << ")(event);\n";
         }
         js_output << "  }\n";
    }
    js_output << "});\n";
}

void CHTLJSGenerator::visit(ListenNode& node) {
    const auto& parsed = node.selector;
    std::string selector_js;
    if (parsed.type == CHTL_JS::SelectorType::IndexedQuery) {
        selector_js = "document.querySelectorAll('" + parsed.selector_string + "')[" + std::to_string(parsed.index.value_or(0)) + "]";
    } else {
        if (!parsed.selector_string.empty() && parsed.selector_string[0] == '#') {
            selector_js = "document.querySelector('" + parsed.selector_string + "')";
        } else {
            selector_js = "document.querySelectorAll('" + parsed.selector_string + "')";
        }
    }

    if (!parsed.selector_string.empty() && parsed.selector_string[0] == '#') {
         for (const auto& event : node.events) {
            js_output << selector_js << ".addEventListener('" << event.first << "', " << event.second << ");\n";
        }
    } else {
        js_output << selector_js << ".forEach(el => {\n";
        for (const auto& event : node.events) {
            js_output << "  el.addEventListener('" << event.first << "', " << event.second << ");\n";
        }
        js_output << "});\n";
    }
}

void CHTLJSGenerator::visit(EventHandlerNode& node) {
    const auto& parsed = node.selector;
    std::string selector_js;
    if (parsed.type == CHTL_JS::SelectorType::IndexedQuery) {
        selector_js = "document.querySelectorAll('" + parsed.selector_string + "')[" + std::to_string(parsed.index.value_or(0)) + "]";
    } else {
        if (!parsed.selector_string.empty() && parsed.selector_string[0] == '#') {
            selector_js = "document.querySelector('" + parsed.selector_string + "')";
        } else {
            selector_js = "document.querySelectorAll('" + parsed.selector_string + "')";
        }
    }
     if (!parsed.selector_string.empty() && parsed.selector_string[0] == '#') {
        for (const auto& event_name : node.event_names) {
            js_output << selector_js << ".addEventListener('" << event_name << "', " << node.handler << ");\n";
        }
    } else {
        js_output << selector_js << ".forEach(el => {\n";
        for (const auto& event_name : node.event_names) {
            js_output << "  el.addEventListener('" << event_name << "', " << node.handler << ");\n";
        }
        js_output << "});\n";
    }
}

void CHTLJSGenerator::visit(RawJSNode& node) {
    js_output << node.content;
}

void CHTLJSGenerator::visit(EnhancedSelectorNode& node) {
    const auto& parsed = node.parsed_selector;
    if (parsed.type == CHTL_JS::SelectorType::IndexedQuery) {
        js_output << "document.querySelectorAll('" << parsed.selector_string << "')[" << parsed.index.value_or(0) << "]";
    } else {
        if (!parsed.selector_string.empty() && parsed.selector_string[0] == '#') {
            js_output << "document.querySelector('" << parsed.selector_string << "')";
        } else {
            js_output << "document.querySelectorAll('" << parsed.selector_string << "')";
        }
    }
}

} // namespace CHTL_JS
