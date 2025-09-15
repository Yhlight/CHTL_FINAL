#include "CHTLJS/CHTLJSGenerator/CHTLJSGenerator.h"
#include "CHTLJS/CHTLJSNode/SequenceNode.h"
#include "CHTLJS/CHTLJSNode/EnhancedSelectorNode.h"
#include "CHTLJS/CHTLJSNode/ListenNode.h"
#include "CHTLJS/CHTLJSNode/DelegateNode.h"
#include "CHTLJS/CHTLJSNode/AnimateNode.h"
#include "CHTLJS/CHTLJSNode/ValueNode.h"
#include "CHTLJS/CHTLJSNode/PlaceholderNode.h"
#include "CHTLJS/CHTLJSNode/VirNode.h"
#include <stdexcept>
#include <sstream>

namespace CHTLJS {

std::string CHTLJSGenerator::generate(const CHTLJSNode& root) {
    return visit(&root);
}

std::string CHTLJSGenerator::visit(const CHTLJSNode* node) {
    if (!node) return "";
    switch (node->getType()) {
        case CHTLJSNodeType::Sequence:
            return visitSequenceNode(static_cast<const SequenceNode*>(node));
        case CHTLJSNodeType::EnhancedSelector:
            return visitEnhancedSelector(static_cast<const EnhancedSelectorNode*>(node));
        case CHTLJSNodeType::Listen:
            return visitListenNode(static_cast<const ListenNode*>(node));
        case CHTLJSNodeType::Delegate:
            return visitDelegateNode(static_cast<const DelegateNode*>(node));
        case CHTLJSNodeType::Animate:
            return visitAnimateNode(static_cast<const AnimateNode*>(node));
        case CHTLJSNodeType::Value:
            return visitValueNode(static_cast<const ValueNode*>(node));
        case CHTLJSNodeType::Placeholder:
            return visitPlaceholderNode(static_cast<const PlaceholderNode*>(node));
        case CHTLJSNodeType::Vir:
            return visitVirNode(static_cast<const VirNode*>(node));
        default:
            throw std::runtime_error("Unknown CHTL JS node type for generation.");
    }
}

std::string CHTLJSGenerator::visitSequenceNode(const SequenceNode* node) {
    std::stringstream ss;
    for (const auto& stmt : node->statements_) {
        ss << visit(stmt.get()) << ";\n";
    }
    return ss.str();
}

std::string CHTLJSGenerator::visitPlaceholderNode(const PlaceholderNode* node) {
    return node->getPlaceholderText();
}

std::string CHTLJSGenerator::visitVirNode(const VirNode* node) {
    return "const " + node->getName() + " = " + visit(node->getValue());
}

std::string CHTLJSGenerator::visitEnhancedSelector(const EnhancedSelectorNode* node) {
    return "document.querySelector(\"" + node->getSelector() + "\")";
}

std::string CHTLJSGenerator::visitListenNode(const ListenNode* node) {
    std::string object_str = visit(node->getObject());
    std::stringstream ss;
    ss << object_str;
    for (const auto& event_pair : node->getEvents()) {
        ss << ".addEventListener('" << event_pair.first << "', " << event_pair.second << ")";
    }
    return ss.str();
}

std::string CHTLJSGenerator::visitDelegateNode(const DelegateNode* node) {
    std::string delegator_str = visit(node->getDelegator());
    std::stringstream ss;

    for (const auto& event_pair : node->getEvents()) {
        ss << delegator_str << ".addEventListener('" << event_pair.first << "', (event) => {\n";
        for (const auto& target_node : node->getTargets()) {
            if(target_node->getType() == CHTLJSNodeType::EnhancedSelector) {
                 std::string target_selector = static_cast<const EnhancedSelectorNode*>(target_node.get())->getSelector();
                 ss << "  if (event.target.matches(\"" << target_selector << "\")) {\n";
                 ss << "    (" << event_pair.second << ")(event);\n";
                 ss << "  }\n";
            }
        }
        ss << "});\n";
    }
    return ss.str();
}

std::string CHTLJSGenerator::visitAnimateNode(const AnimateNode* node) {
    // 1. Generate the keyframes array
    std::stringstream keyframes_ss;
    keyframes_ss << "[";
    bool first_frame = true;
    for (const auto& kf : node->keyframes_) {
        if (!first_frame) keyframes_ss << ", ";
        keyframes_ss << "{ ";
        if (kf.offset.has_value()) {
            keyframes_ss << "offset: " << kf.offset.value();
        }
        bool first_prop = !kf.offset.has_value();
        for (const auto& prop_pair : kf.properties) {
            if (!first_prop) keyframes_ss << ", ";
            keyframes_ss << "'" << prop_pair.first << "': '" << prop_pair.second << "'";
            first_prop = false;
        }
        keyframes_ss << " }";
        first_frame = false;
    }
    keyframes_ss << "]";

    // 2. Generate the options object
    std::stringstream options_ss;
    options_ss << "{ ";
    bool first_option = true;
    auto add_option = [&](const std::string& name, const std::optional<std::string>& val, bool is_string) {
        if (val.has_value()) {
            if (!first_option) options_ss << ", ";
            options_ss << name << ": " << (is_string ? "'" : "") << val.value() << (is_string ? "'" : "");
            first_option = false;
        }
    };
    add_option("duration", node->duration_, false);
    add_option("easing", node->easing_, true);
    add_option("iterations", node->loop_, false);
    add_option("direction", node->direction_, true);
    add_option("delay", node->delay_, false);
    options_ss << " }";

    // 3. Generate the final .animate() call on the target(s)
    std::stringstream final_ss;
    for (const auto& target : node->targets_) {
        final_ss << visit(target.get());
        final_ss << ".animate(" << keyframes_ss.str() << ", " << options_ss.str() << ")";
        if (node->callback_.has_value()) {
            final_ss << ".addEventListener('finish', " << node->callback_.value() << ");\n";
        } else {
            final_ss << ";\n";
        }
    }
    return final_ss.str();
}

std::string CHTLJSGenerator::visitValueNode(const ValueNode* node) {
    return node->getValue();
}

} // namespace CHTLJS
