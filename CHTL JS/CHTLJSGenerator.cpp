#include "CHTLJSGenerator.h"

namespace CHTL_JS {

CHTLJSGenerator::CHTLJSGenerator() {}

CHTLJSCompilationResult CHTLJSGenerator::generate(const std::vector<std::unique_ptr<CHTLJSBaseNode>>& nodes) {
    CHTLJSCompilationResult result;
    js_output.str("");
    for (const auto& node : nodes) {
        visit(node.get(), result);
    }
    result.js = js_output.str();
    return result;
}

void CHTLJSGenerator::visit(CHTLJSBaseNode* node, CHTLJSCompilationResult& result) {
    if (!node) return;
    switch (node->type) {
        case CHTLJSNodeType::Animate:
            visitAnimateNode(static_cast<AnimateNode*>(node));
            break;
        case CHTLJSNodeType::Listen:
            visitListenNode(static_cast<ListenNode*>(node));
            break;
        case CHTLJSNodeType::Delegate:
            result.delegate_nodes.push_back(*static_cast<DelegateNode*>(node));
            break;
        case CHTLJSNodeType::EventHandler:
            visitEventHandlerNode(static_cast<EventHandlerNode*>(node));
            break;
        case CHTLJSNodeType::RawJS:
            visitRawJSNode(static_cast<RawJSNode*>(node));
            break;
        case CHTLJSNodeType::EnhancedSelector:
            visitEnhancedSelectorNode(static_cast<EnhancedSelectorNode*>(node));
            break;
    }
}

void CHTLJSGenerator::visitAnimateNode(AnimateNode* node) {
    js_output << "{\n";
    js_output << "  const targets = [";
    for (size_t i = 0; i < node->targets.size(); ++i) {
        js_output << "document.querySelector('" << node->targets[i].selector_string << "')";
        if (i < node->targets.size() - 1) js_output << ", ";
    }
    js_output << "];\n";
    js_output << "  const duration = " << node->duration.value_or(1000) << ";\n";
    // ... (generate other properties)
    js_output << "  let startTime = null;\n";
    js_output << "  function step(timestamp) {\n";
    js_output << "    if (!startTime) startTime = timestamp;\n";
    js_output << "    const progress = Math.min((timestamp - startTime) / duration, 1);\n";
    js_output << "    targets.forEach(target => {\n";
    // ... (generate style updates based on progress)
    js_output << "    });\n";
    js_output << "    if (progress < 1) {\n";
    js_output << "      requestAnimationFrame(step);\n";
    js_output << "    }\n";
    js_output << "  }\n";
    js_output << "  requestAnimationFrame(step);\n";
    js_output << "}\n";
}

void CHTLJSGenerator::visitListenNode(ListenNode* node) {
    const auto& parsed = node->selector;
    std::string selector_js;
    if (parsed.type == SelectorType::IndexedQuery) {
        selector_js = "document.querySelectorAll('" + parsed.selector_string + "')[" + std::to_string(parsed.index.value_or(0)) + "]";
    } else {
        if (!parsed.selector_string.empty() && parsed.selector_string[0] == '#') {
            selector_js = "document.querySelector('" + parsed.selector_string + "')";
        } else {
            selector_js = "document.querySelectorAll('" + parsed.selector_string + "')";
        }
    }

    if (!parsed.selector_string.empty() && parsed.selector_string[0] == '#') {
         for (const auto& event : node->events) {
            js_output << selector_js << ".addEventListener('" << event.first << "', " << event.second << ");\n";
        }
    } else {
        js_output << selector_js << ".forEach(el => {\n";
        for (const auto& event : node->events) {
            js_output << "  el.addEventListener('" << event.first << "', " << event.second << ");\n";
        }
        js_output << "});\n";
    }
}

void CHTLJSGenerator::visitDelegateNode(DelegateNode* node) {
    // This logic needs to be handled by the CodeMerger, as it requires a global view.
    // For now, we'll just generate a comment.
    js_output << "/* Delegate event for " << node->parent_selector.selector_string << " */\n";
}

void CHTLJSGenerator::visitEventHandlerNode(EventHandlerNode* node) {
    const auto& parsed = node->selector;
    std::string selector_js;
    if (parsed.type == SelectorType::IndexedQuery) {
        selector_js = "document.querySelectorAll('" + parsed.selector_string + "')[" + std::to_string(parsed.index.value_or(0)) + "]";
    } else {
        if (!parsed.selector_string.empty() && parsed.selector_string[0] == '#') {
            selector_js = "document.querySelector('" + parsed.selector_string + "')";
        } else {
            selector_js = "document.querySelectorAll('" + parsed.selector_string + "')";
        }
    }
     if (!parsed.selector_string.empty() && parsed.selector_string[0] == '#') {
        for (const auto& event_name : node->event_names) {
            js_output << selector_js << ".addEventListener('" << event_name << "', " << node->handler << ");\n";
        }
    } else {
        js_output << selector_js << ".forEach(el => {\n";
        for (const auto& event_name : node->event_names) {
            js_output << "  el.addEventListener('" << event_name << "', " << node->handler << ");\n";
        }
        js_output << "});\n";
    }
}

void CHTLJSGenerator::visitRawJSNode(RawJSNode* node) {
    js_output << node->content;
}

void CHTLJSGenerator::visitEnhancedSelectorNode(EnhancedSelectorNode* node) {
    const auto& parsed = node->parsed_selector;
    if (parsed.type == SelectorType::IndexedQuery) {
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
