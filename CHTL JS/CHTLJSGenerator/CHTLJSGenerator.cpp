#include "CHTLJSGenerator.h"
#include "../CHTLJSNode/JSCodeNode.h"
#include "../CHTLJSNode/EnhancedSelectorNode.h"
#include <sstream>

namespace CHTL_JS {

CHTLJSGenerator::CHTLJSGenerator(NodeList nodes) : m_nodes(std::move(nodes)) {}

std::string CHTLJSGenerator::Generate() {
    std::stringstream output;
    for (const auto& node : m_nodes) {
        if (auto js_node = std::dynamic_pointer_cast<JSCodeNode>(node)) {
            output << js_node->content;
        } else if (auto selector_node = std::dynamic_pointer_cast<EnhancedSelectorNode>(node)) {
            // A more robust implementation would parse the selector string itself.
            // For now, we just wrap it.
            output << "document.querySelector('" << selector_node->selector << "')";
        }
    }
    return output.str();
}

} // namespace CHTL_JS
