#pragma once

#include "ast/ast_node.h"
#include "style/style_processor.h"
#include <memory>

namespace chtl {
namespace style {

class StyleAggregator {
public:
    StyleAggregator();

    // Traverses the AST and processes all local style blocks
    void aggregate_styles(ast::ASTNode::NodePtr root);

private:
    void visit_node(ast::ASTNode::NodePtr node);

    StyleProcessor processor_;
};

} // namespace style
} // namespace chtl
