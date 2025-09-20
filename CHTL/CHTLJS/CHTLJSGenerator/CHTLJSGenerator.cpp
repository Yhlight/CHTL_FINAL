#include "CHTLJSGenerator.h"
#include "../CHTLJSNode/CHTLJS_ListenNode.h"
#include "../CHTLJSNode/CHTLJS_RawJSNode.h"
#include <stdexcept>

namespace CHTLJS {

std::string CHTLJSGenerator::generate(const std::vector<std::unique_ptr<CHTLJS_BaseNode>>& nodes) {
    for (const auto& node : nodes) {
        if (node) {
            node->accept(*this);
        }
    }
    return output;
}

void CHTLJSGenerator::visit(CHTLJS_SelectorNode& node) {
    output += "document.querySelector('" + node.selector + "')";
}

void CHTLJSGenerator::visit(CHTLJS_ListenNode& node) {
    if (node.target) {
        node.target->accept(*this);
    }

    for (const auto& handler : node.handlers) {
        output += ".addEventListener('" + handler.event_name + "', " + handler.handler_code + ")";
    }
}

void CHTLJSGenerator::visit(CHTLJS_RawJSNode& node) {
    output += node.js_code;
}

} // namespace CHTLJS
