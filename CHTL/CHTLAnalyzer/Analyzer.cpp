#include "Analyzer.h"

namespace CHTL {

Analyzer::Analyzer() {}

void Analyzer::analyze(RootNode* root) {
    if (root) {
        visit(root);
    }
}

void Analyzer::visit(Node* node) {
    if (!node) return;

    // We need pre-order traversal: process the node, then its children.
    // This ensures that when we visit an ElementNode, we can modify it
    // before its children are processed (though not strictly necessary for this feature).

    if (node->getType() == NodeType::Element) {
        visitElementNode(static_cast<ElementNode*>(node));
    }

    // Continue traversal for all node types.
    for (const auto& child : node->getChildren()) {
        visit(child.get());
    }
}

void Analyzer::visitElementNode(ElementNode* node) {
    // The CHTL spec says to add the class/id if the attribute is missing.
    bool has_class_attr = node->getAttributes().count("class");
    bool has_id_attr = node->getAttributes().count("id");

    // Find StyleNode children and inspect their rules.
    for (const auto& child : node->getChildren()) {
        if (child->getType() == NodeType::Style) {
            auto* style_node = static_cast<StyleNode*>(child.get());
            for (const auto& rule_child : style_node->getChildren()) {
                if (rule_child->getType() == NodeType::CssRule) {
                    auto* css_rule = static_cast<CssRuleNode*>(rule_child.get());
                    const std::string& selector = css_rule->getSelector();

                    if (selector.empty()) continue;

                    // Handle class selectors
                    if (selector[0] == '.' && !has_class_attr) {
                        std::string className = selector.substr(1);
                        node->setAttribute("class", className);
                        has_class_attr = true; // Stop after finding the first one.
                    }
                    // Handle ID selectors
                    else if (selector[0] == '#' && !has_id_attr) {
                        std::string idName = selector.substr(1);
                        node->setAttribute("id", idName);
                        has_id_attr = true; // Stop after finding the first one.
                    }
                }
            }
        }
    }
}

} // namespace CHTL
