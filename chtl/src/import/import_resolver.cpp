#include "import/import_resolver.h"
#include "ast/ast_node.h"

namespace chtl {
namespace import {

ImportResolver::ImportResolver(ImportManager& manager)
    : manager_(manager) {
}

void ImportResolver::resolve_imports(ast::ASTNode::NodePtr root) {
    if (!root) {
        return;
    }
    visit_node(root);
}

void ImportResolver::visit_node(ast::ASTNode::NodePtr node) {
    if (!node) {
        return;
    }

    if (node->type == ast::NodeType::IMPORT_CHTL ||
        node->type == ast::NodeType::IMPORT_HTML ||
        node->type == ast::NodeType::IMPORT_STYLE ||
        node->type == ast::NodeType::IMPORT_JAVASCRIPT ||
        node->type == ast::NodeType::IMPORT_CJMOD) {

        auto import_node = std::static_pointer_cast<ast::ImportNode>(node);
        // The manager will handle the actual loading and parsing
        manager_.process_import_node(import_node);
    }

    // Recurse on children
    for (const auto& child : node->children) {
        visit_node(child);
    }
}

} // namespace import
} // namespace chtl
