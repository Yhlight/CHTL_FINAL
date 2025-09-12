#include "StyleEvaluator.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CssValueParser/Evaluator.h"
#include <iostream>
#include <sstream>

void StyleEvaluator::process(std::shared_ptr<BaseNode> root) {
    if (root) {
        this->astRoot = root;
        root->accept(*this);
    }
}

void StyleEvaluator::visit(ElementNode& node) {
    // Evaluate the stored expression trees for this node
    for (auto const& [key, exprTree] : node.unevaluatedStyles) {
        if (exprTree) {
            Evaluator evaluator;
            // The evaluator needs the AST root to resolve property references.
            EvaluatedValue evaluated = evaluator.evaluate(exprTree, this->astRoot);

            if (evaluated.hasError) {
                std::cerr << "Warning: Could not evaluate style property '" << key << "'. Error: " << evaluated.errorMessage << std::endl;
            } else {
                std::stringstream ss;
                ss << evaluated.value << evaluated.unit;
                node.processedStyles[key] = ss.str();
            }
        }
    }

    // Recursively visit children
    for (const auto& child : node.children) {
        child->accept(*this);
    }
}

void StyleEvaluator::visit(TextNode& node) {
    // Do nothing
}

void StyleEvaluator::visit(CommentNode& node) {
    // Do nothing
}

void StyleEvaluator::visit(StyleNode& node) {
    // Should not exist at this stage.
}
