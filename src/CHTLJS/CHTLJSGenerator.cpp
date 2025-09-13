#include "CHTLJSGenerator.h"
#include <sstream>

namespace CHTL {
namespace CHTLJS {

// Forward declare a helper to avoid circular dependencies if it were in the header
std::string generateNode(const JSNode* node);

std::string CHTLJSGenerator::generate(const std::vector<std::unique_ptr<JSNode>>& ast) {
    std::stringstream ss;
    for (const auto& node : ast) {
        ss << generateNode(node.get());
    }
    return ss.str();
}

std::string generateNode(const JSNode* node) {
    std::stringstream ss;
    if (node->getType() == JSNodeType::Script) {
        ss << static_cast<const StandardJSNode*>(node)->content_;
    } else if (node->getType() == JSNodeType::EnhancedSelector) {
        auto* selectorNode = static_cast<const EnhancedSelectorNode*>(node);
        // Trim whitespace from the selector string
        std::string selector = selectorNode->selector_;
        selector.erase(0, selector.find_first_not_of(" \t\n\r"));
        selector.erase(selector.find_last_not_of(" \t\n\r") + 1);
        ss << "document.querySelector('" << selector << "')";
    } else if (node->getType() == JSNodeType::Listen) {
        auto* listenNode = static_cast<const ListenNode*>(node);
        std::string target_js = generateNode(listenNode->target_.get());
        for (const auto& event_pair : listenNode->events) {
            ss << target_js << ".addEventListener('" << event_pair.first << "', " << event_pair.second << ");\n";
        }
    }
    return ss.str();
}


} // namespace CHTLJS
} // namespace CHTL
