#include "style/style_aggregator.h"
#include "ast/ast_node.h"
#include <vector>

namespace chtl {
namespace style {

StyleAggregator::StyleAggregator() {
}

void StyleAggregator::aggregate_styles(ast::ASTNode::NodePtr root) {
    if (!root) {
        return;
    }
    visit_node(root);
}

void StyleAggregator::visit_node(ast::ASTNode::NodePtr node) {
    if (!node) {
        return;
    }

    // If the node is an element, check for style children
    if (node->type == ast::NodeType::ELEMENT) {
        auto element_node = std::static_pointer_cast<ast::ElementNode>(node);
        std::vector<ast::ASTNode::NodePtr> style_nodes_to_remove;

        for (const auto& child : element_node->children) {
            if (child->type == ast::NodeType::STYLE) {
                auto style_node = std::static_pointer_cast<ast::StyleNode>(child);

                // Reset context for each style block
                processor_.reset_context();
                processor_.process_style_block(style_node->value, "");

                const auto& context = processor_.get_context();

                // Apply inline styles
                if (context.has_attribute("inline_style")) {
                    std::string existing_style = element_node->get_attribute("style") ? element_node->get_attribute("style")->value : "";
                    std::string new_style = existing_style + context.get_attribute("inline_style");
                    element_node->set_attribute("style", std::make_shared<ast::LiteralNode>('"' + new_style + '"'));
                }

                // Apply classes
                if (context.has_attribute("class")) {
                    std::string existing_class = element_node->get_attribute("class") ? element_node->get_attribute("class")->value : "";
                    std::string new_class = existing_class + (existing_class.empty() ? "" : " ") + context.get_attribute("class");
                    element_node->set_attribute("class", std::make_shared<ast::LiteralNode>('"' + new_class + '"'));
                }

                // Apply IDs
                if (context.has_attribute("id")) {
                    // Note: Multiple IDs are not valid HTML, but we'll just append for now.
                    // A real implementation should warn or override.
                    std::string existing_id = element_node->get_attribute("id") ? element_node->get_attribute("id")->value : "";
                    std::string new_id = existing_id + (existing_id.empty() ? "" : " ") + context.get_attribute("id");
                    element_node->set_attribute("id", std::make_shared<ast::LiteralNode>('"' + new_id + '"'));
                }

                style_nodes_to_remove.push_back(child);
            }
        }

        // Remove the processed style nodes
        for (const auto& style_node : style_nodes_to_remove) {
            element_node->remove_child(style_node);
        }
    }

    // Recurse on children
    for (const auto& child : node->children) {
        visit_node(child);
    }
}

} // namespace style
} // namespace chtl
