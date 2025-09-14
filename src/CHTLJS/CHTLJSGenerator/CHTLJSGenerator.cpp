#include "CHTLJS/CHTLJSGenerator/CHTLJSGenerator.h"
#include "CHTLJS/CHTLJSNode/EnhancedSelectorNode.h"
#include "CHTLJS/CHTLJSNode/ListenNode.h"
#include "CHTLJS/CHTLJSNode/DelegateNode.h"
#include "CHTLJS/CHTLJSNode/AnimateNode.h"
#include "CHTLJS/CHTLJSNode/ValueNode.h"
#include <stdexcept>
#include <sstream>

namespace CHTL {
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
        case CHTLJSNodeType::Animate:
            visitAnimateNode(static_cast<const AnimateNode*>(node));
            break;
        case CHTLJSNodeType::Value:
            visitValueNode(static_cast<const ValueNode*>(node));
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

void CHTLJSGenerator::visitAnimateNode(const AnimateNode* node) {
    // 1. Generate the keyframes array
    std::stringstream keyframes_ss;
    keyframes_ss << "[";
    bool first_frame = true;
    for (const auto& kf : node->keyframes_) {
        if (!first_frame) keyframes_ss << ", ";
        keyframes_ss << "{ ";
        if (kf.offset.has_value()) {
            keyframes_ss << "offset: " << kf.offset.value() << ", ";
        }
        bool first_prop = true;
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
    if (node->duration_.has_value()) {
        options_ss << "duration: " << node->duration_.value();
        first_option = false;
    }
    if (node->easing_.has_value()) {
        if (!first_option) options_ss << ", ";
        options_ss << "easing: '" << node->easing_.value() << "'";
        first_option = false;
    }
    if (node->loop_.has_value()) {
        if (!first_option) options_ss << ", ";
        options_ss << "iterations: " << node->loop_.value();
        first_option = false;
    }
    if (node->direction_.has_value()) {
        if (!first_option) options_ss << ", ";
        options_ss << "direction: '" << node->direction_.value() << "'";
        first_option = false;
    }
    if (node->delay_.has_value()) {
        if (!first_option) options_ss << ", ";
        options_ss << "delay: " << node->delay_.value();
        first_option = false;
    }
    options_ss << " }";

    // 3. Generate the final .animate() call on the target(s)
    for (const auto& target : node->targets_) {
        visit(target.get()); // generates the document.querySelector(...)
        output_ += ".animate(" + keyframes_ss.str() + ", " + options_ss.str() + ")";
        if (node->callback_.has_value()) {
            output_ += ".addEventListener('finish', " + node->callback_.value() + ");\n";
        } else {
            output_ += ";\n";
        }
    }
}

void CHTLJSGenerator::visitValueNode(const ValueNode* node) {
    output_ += node->getValue();
}


} // namespace CHTLJS
} // namespace CHTL
