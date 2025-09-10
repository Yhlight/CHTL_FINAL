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
    visit_node(root, nullptr);
}

void ImportResolver::visit_node(ast::ASTNode::NodePtr node, ast::ASTNode::NodePtr parent) {
    if (!node) {
        return;
    }

    // We need to iterate carefully as we might modify the children list
    for (size_t i = 0; i < node->children.size(); ++i) {
        auto child = node->children[i];

        if (child->type == ast::NodeType::IMPORT_CHTL) {
            auto import_node = std::static_pointer_cast<ast::ImportNode>(child);

            // Check if it's a module import (no file extension)
            std::filesystem::path import_path(import_node->file_path);
            if (!import_path.has_extension()) {
                std::string module_name = import_node->file_path;
                auto module_ast = manager_.load_module(module_name);

                if (module_ast) {
                    // Replace the import node with the module's AST content
                    node->children.erase(node->children.begin() + i);
                    for (const auto& module_child : module_ast->children) {
                        node->children.insert(node->children.begin() + i, module_child);
                        module_child->parent = node; // Set parent pointer
                        i++;
                    }
                    i--; // Decrement i to re-evaluate the current position
                }
                continue; // Continue to the next child
            }
        }

        // Recurse on the original child if it wasn't replaced
        visit_node(child, node);
    }
}

} // namespace import
} // namespace chtl
