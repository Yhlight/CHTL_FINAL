#pragma once

#include "ast/ast_node.h"
#include "import/import_manager.h"
#include <memory>

namespace chtl {
namespace import {

class ImportResolver {
public:
    explicit ImportResolver(ImportManager& manager);

    // Traverses the AST and resolves all import nodes
    void resolve_imports(ast::ASTNode::NodePtr root);

private:
    void visit_node(ast::ASTNode::NodePtr node, ast::ASTNode::NodePtr parent);

    ImportManager& manager_;
};

} // namespace import
} // namespace chtl
