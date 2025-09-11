#include "ASTCloner.h"

// --- Expression Cloner ---
// A separate visitor for cloning expression trees.
class ExprCloner : public ExprVisitor {
public:
    ExprNodePtr clone(ExprNode* expr) {
        if (!expr) return nullptr;
        expr->accept(*this);
        return std::move(cloned_expr);
    }

    void visit(LiteralExprNode* node) override {
        cloned_expr = std::make_unique<LiteralExprNode>(node->value);
    }

    void visit(BinaryExprNode* node) override {
        auto left = clone(node->left.get());
        auto right = clone(node->right.get());
        cloned_expr = std::make_unique<BinaryExprNode>(std::move(left), node->op, std::move(right));
    }

    void visit(TernaryExprNode* node) override {
        auto cond = clone(node->condition.get());
        auto trueExpr = clone(node->trueExpr.get());
        auto falseExpr = clone(node->falseExpr.get());
        cloned_expr = std::make_unique<TernaryExprNode>(std::move(cond), std::move(trueExpr), std::move(falseExpr));
    }

private:
    ExprNodePtr cloned_expr;
};


// --- AST Cloner ---

NodePtr ASTCloner::clone(Node* node) {
    if (!node) return nullptr;
    node->accept(*this);
    return std::move(cloned_node);
}

NodeList ASTCloner::clone(const NodeList& nodes) {
    NodeList new_nodes;
    for (const auto& node : nodes) {
        new_nodes.push_back(clone(node.get()));
    }
    return new_nodes;
}

void ASTCloner::visit(ElementNode* node) {
    auto new_element = std::make_unique<ElementNode>(node->tagName);
    new_element->children = clone(node->children);
    cloned_node = std::move(new_element);
}

void ASTCloner::visit(TextNode* node) {
    cloned_node = std::make_unique<TextNode>(node->text);
}

void ASTCloner::visit(CommentNode* node) {
    cloned_node = std::make_unique<CommentNode>(node->text, node->isGeneratorComment);
}

void ASTCloner::visit(PropertyNode* node) {
    ExprCloner cloner;
    auto new_value = cloner.clone(node->value.get());
    cloned_node = std::make_unique<PropertyNode>(node->name, std::move(new_value));
}

void ASTCloner::visit(StyleNode* node) {
    auto new_style = std::make_unique<StyleNode>();
    new_style->children = clone(node->children);
    cloned_node = std::move(new_style);
}
