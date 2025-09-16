#include "CHTLJSGenerator.h"

#include "../CHTLJSNode/AnimateNode.h"
#include "../CHTLJSNode/DelegateNode.h"
#include "../CHTLJSNode/EnhancedSelectorNode.h"
#include "../CHTLJSNode/ListenNode.h"
#include "../CHTLJSNode/PlaceholderNode.h"
#include "../CHTLJSNode/SequenceNode.h"
#include "../CHTLJSNode/ValueNode.h"
#include "../CHTLJSNode/VirNode.h"

#include <stdexcept>
#include <iostream>

namespace CHTLJS {

std::string CHTLJSGenerator::generate(const CHTLJSNode& root, const std::map<std::string, std::string>& placeholder_map) {
    this->placeholder_map_ = &placeholder_map;
    return visit(&root);
}

std::string CHTLJSGenerator::visit(const CHTLJSNode* node) {
    if (!node) {
        return "";
    }

    switch (node->getType()) {
        case CHTLJSNodeType::Sequence:
            return visitSequenceNode(static_cast<const SequenceNode*>(node));
        case CHTLJSNodeType::Placeholder:
            return visitPlaceholderNode(static_cast<const PlaceholderNode*>(node));
        case CHTLJSNodeType::EnhancedSelector:
            return visitEnhancedSelector(static_cast<const EnhancedSelectorNode*>(node));
        case CHTLJSNodeType::Listen:
            return visitListenNode(static_cast<const ListenNode*>(node));
        case CHTLJSNodeType::Value:
            return visitValueNode(static_cast<const ValueNode*>(node));
        case CHTLJSNodeType::Animate:
             return "/* AnimateNode not fully implemented */";
        case CHTLJSNodeType::Delegate:
             return "/* DelegateNode not fully implemented */";
        case CHTLJSNodeType::Vir:
             return "/* VirNode not fully implemented */";
        default:
            throw std::runtime_error("Unknown CHTLJSNode type in generator.");
    }
}

std::string CHTLJSGenerator::visitSequenceNode(const SequenceNode* node) {
    std::string result;
    for (const auto& statement : node->statements_) {
        result += visit(statement.get());
    }
    return result;
}

std::string CHTLJSGenerator::visitPlaceholderNode(const PlaceholderNode* node) {
    if (!placeholder_map_) {
        throw std::runtime_error("Placeholder map is not set in the generator.");
    }
    auto it = placeholder_map_->find(node->getPlaceholderText());
    if (it == placeholder_map_->end()) {
        // This should not happen if the scanner and parser are correct.
        throw std::runtime_error("Could not find placeholder in map: " + node->getPlaceholderText());
    }
    return it->second;
}

std::string CHTLJSGenerator::visitEnhancedSelector(const EnhancedSelectorNode* node) {
    // This is a simplified transformation. A real one would need more robust parsing.
    const std::string& selector = node->getSelector();
    return "document.querySelector('" + selector + "')";
}

std::string CHTLJSGenerator::visitListenNode(const ListenNode* node) {
    std::string target_js = visit(node->getObject());
    std::string result;
    for (const auto& pair : node->getEvents()) {
        const std::string& event_name = pair.first;
        const std::string& callback_placeholder = pair.second;

        if (placeholder_map_->find(callback_placeholder) == placeholder_map_->end()) {
            throw std::runtime_error("Could not find callback placeholder in map: " + callback_placeholder);
        }
        std::string callback_js = placeholder_map_->at(callback_placeholder);

        result += target_js + ".addEventListener('" + event_name + "', " + callback_js + ");\n";
    }
    return result;
}

std::string CHTLJSGenerator::visitValueNode(const ValueNode* node) {
    return node->getValue();
}

} // namespace CHTLJS
