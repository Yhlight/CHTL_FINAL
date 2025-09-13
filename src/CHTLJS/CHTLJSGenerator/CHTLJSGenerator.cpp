#include "CHTLJS/CHTLJSGenerator/CHTLJSGenerator.h"
#include "CHTLJS/CHTLJSNode/EnhancedSelectorNode.h"
#include "CHTLJS/CHTLJSNode/ListenNode.h"
#include "CHTLJS/CHTLJSNode/DelegateNode.h"
#include <stdexcept>
#include <sstream>

namespace CHTLJS {

CHTLJSGenerator::CHTLJSGenerator() {}

std::string CHTLJSGenerator::generate(const CHTLJSNode& root) {
    visit(&root);
    return output_;
}

void CHTLJSGenerator::visit(const CHTLJSNode* node) {
    if (!node) return;
    switch (node->getType()) {
        case CHTLJSNodeType::EnhancedSelector:
            visitEnhancedSelector(static_cast<const EnhancedSelectorNode*>(node));
            break;
        case CHTLJSNodeType::Listen:
            visitListenNode(static_cast<const ListenNode*>(node));
            break;
        case CHTLJSNodeType::Delegate:
            visitDelegateNode(static_cast<const DelegateNode*>(node));
            break;
        default:
            throw std::runtime_error("Unknown CHTL JS node type for generation.");
    }
}

void CHTLJSGenerator::visitEnhancedSelector(const EnhancedSelectorNode* node) {
    output_ += "document.querySelector(\"" + node->getSelector() + "\")";
}

void CHTLJSGenerator::visitListenNode(const ListenNode* node) {
    visit(node->getObject());
    for (const auto& event_pair : node->getEvents()) {
        output_ += ".addEventListener('" + event_pair.first + "', " + event_pair.second + ");";
    }
}

void CHTLJSGenerator::visitDelegateNode(const DelegateNode* node) {
    std::stringstream delegator_stream;
    std::string original_output = output_;
    output_ = "";
    visit(node->getDelegator());
    delegator_stream << output_;
    output_ = original_output;

    for (const auto& event_pair : node->getEvents()) {
        output_ += delegator_stream.str() + ".addEventListener('" + event_pair.first + "', (event) => {\n";

        for (const auto& target_node : node->getTargets()) {
            std::stringstream target_selector_stream;
            std::string temp_output = output_;
            output_ = "";
            // We assume targets are EnhancedSelectorNodes which generate a selector string.
            // A more robust implementation would handle other node types.
            if(target_node->getType() == CHTLJSNodeType::EnhancedSelector) {
                 target_selector_stream << "\"" << static_cast<const EnhancedSelectorNode*>(target_node.get())->getSelector() << "\"";
            }
            output_ = temp_output;

            output_ += "  if (event.target.matches(" + target_selector_stream.str() + ")) {\n";
            output_ += "    (" + event_pair.second + ")(event);\n";
            output_ += "  }\n";
        }

        output_ += "});\n";
    }
}


} // namespace CHTLJS
