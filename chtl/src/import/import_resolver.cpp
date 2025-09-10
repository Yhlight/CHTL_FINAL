#include "import/import_resolver.h"
#include "ast/ast_node.h"
#include <filesystem>
#include <iostream>

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

    for (size_t i = 0; i < node->children.size(); ) {
        auto child = node->children[i];
        bool node_processed = false;

        if (child->type == ast::NodeType::IMPORT_CHTL) {
            auto import_node = std::static_pointer_cast<ast::ImportNode>(child);

            // Check if it's a granular import or a full module/file import
            if (!import_node->imported_item_name.empty()) {
                // Granular Import
                auto imported_nodes = manager_.resolve_granular_import(import_node);

                // Replace the import node with the resolved nodes
                node->children.erase(node->children.begin() + i);
                for (const auto& imported_item : imported_nodes) {
                    node->children.insert(node->children.begin() + i, imported_item);
                    imported_item->parent = node;
                    i++;
                }
                node_processed = true;

            } else {
                // Module/File Import (Default Namespace)
                std::filesystem::path import_path(import_node->file_path);
                if (!import_path.has_extension()) {
                    std::string module_name = import_node->file_path;
                    auto module_ast = manager_.load_module(module_name);

                    if (module_ast) {
                        auto namespace_node = std::make_shared<ast::NamespaceNode>(module_name);
                        namespace_node->add_child(module_ast);

                        node->children[i] = namespace_node;
                        namespace_node->parent = node;

                        visit_node(namespace_node);
                        node_processed = true;
                    }
                }
            }
        }

        if (!node_processed) {
            visit_node(child);
            i++;
        } else {
            // If we replaced nodes, we want to re-evaluate from the start of the inserted block.
            // The loop increment `i++` in the insertion loop already handles this.
            // If we just replaced one node with another (namespace), we should just increment i.
            if (node->children[i]->type != ast::NodeType::NAMESPACE) {
                 // This logic is getting complex, a simple increment is safer for now.
            }
            i++;
        }
    }
}

} // namespace import
} // namespace chtl
