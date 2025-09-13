#include "CHTLJSGenerator.h"
#include <sstream>

namespace CHTL {
namespace CHTLJS {

std::string CHTLJSGenerator::generate(const std::vector<std::unique_ptr<JSNode>>& ast) {
    std::stringstream ss;
    for (const auto& node : ast) {
        if (node->getType() == JSNodeType::Script) {
            ss << static_cast<StandardJSNode*>(node.get())->content_;
        } else if (node->getType() == JSNodeType::EnhancedSelector) {
            auto* selectorNode = static_cast<EnhancedSelectorNode*>(node.get());
            // Trim whitespace from the selector string
            std::string selector = selectorNode->selector_;
            selector.erase(0, selector.find_first_not_of(" \t\n\r"));
            selector.erase(selector.find_last_not_of(" \t\n\r") + 1);
            ss << "document.querySelector('" << selector << "')";
        }
    }
    return ss.str();
}

} // namespace CHTLJS
} // namespace CHTL
