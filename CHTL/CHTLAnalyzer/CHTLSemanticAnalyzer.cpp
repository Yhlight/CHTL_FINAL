#include "CHTLSemanticAnalyzer.h"
#include "../CHTLNode/AttributeNode.h"
#include <algorithm>

namespace CHTL {

void CHTLSemanticAnalyzer::Process(const NodePtr& root) {
    visit(root);
}

void CHTLSemanticAnalyzer::visit(const NodePtr& node) {
    if (!node) return;

    if (auto element = std::dynamic_pointer_cast<ElementNode>(node)) {
        visitElementNode(element);
    }
}

void CHTLSemanticAnalyzer::visitElementNode(const std::shared_ptr<ElementNode>& node) {
    // First, visit all children to process them recursively.
    for (const auto& child : node->children) {
        visit(child);
    }

    // Now, process the current node.
    // Pass 1: Add automatic class/id attributes from selectors like .box and #id
    for (const auto& child : node->children) {
        if (auto styleNode = std::dynamic_pointer_cast<StyleNode>(child)) {
            for (const auto& selectorBlock : styleNode->selector_blocks) {
                const std::string& selector = selectorBlock->selector;
                if (selector.empty() || selector[0] == '&') continue; // Skip & selectors for now

                char selectorType = selector[0];
                std::string selectorName = selector.substr(1);

                if (selectorType == '.') {
                    AttributePtr classAttr = nullptr;
                    for (auto& attr : node->attributes) {
                        if (attr->key == "class") {
                            classAttr = attr;
                            break;
                        }
                    }
                    if (classAttr) {
                        if (classAttr->value.find(selectorName) == std::string::npos) {
                            classAttr->value += " " + selectorName;
                        }
                    } else {
                        node->attributes.push_back(std::make_shared<AttributeNode>("class", selectorName));
                    }
                } else if (selectorType == '#') {
                    node->attributes.push_back(std::make_shared<AttributeNode>("id", selectorName));
                }
            }
        }
    }

    // Pass 2: Resolve '&' selectors using the now-populated class/id attributes.
    for (const auto& child : node->children) {
        if (auto styleNode = std::dynamic_pointer_cast<StyleNode>(child)) {
            for (const auto& selectorBlock : styleNode->selector_blocks) {
                std::string& selector = selectorBlock->selector; // Get a mutable reference
                if (selector.empty() || selector[0] != '&') continue;

                // Find class attribute (preferred)
                std::string replacement = "";
                for (auto& attr : node->attributes) {
                    if (attr->key == "class") {
                        // Just use the first class for simplicity. A more complex system could handle multiple.
                        std::string firstClass = attr->value.substr(0, attr->value.find(" "));
                        replacement = "." + firstClass;
                        break;
                    }
                }
                // If no class, find id attribute
                if (replacement.empty()) {
                     for (auto& attr : node->attributes) {
                        if (attr->key == "id") {
                            replacement = "#" + attr->value;
                            break;
                        }
                    }
                }

                if (!replacement.empty()) {
                    selector.replace(0, 1, replacement);
                } else {
                    // Could log a warning here: '&' used without a class or id.
                }
            }
        }
    }
}

} // namespace CHTL
