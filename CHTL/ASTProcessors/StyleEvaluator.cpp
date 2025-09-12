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
            std::set<std::string> evaluationStack; // For circular dependency detection
            // The evaluator needs the AST root to resolve property references.
            EvaluatedValue evaluated = evaluator.evaluate(exprTree, this->astRoot, &node, evaluationStack);

            if (evaluated.hasError) {
                std::cerr << "Warning: Could not evaluate style property '" << key << "'. Error: " << evaluated.errorMessage << std::endl;
            } else {
                // Convert the variant result to a string
                std::string finalValue;
                if (const auto* numWithUnit = std::get_if<NumberWithUnit>(&evaluated.value)) {
                    std::stringstream ss;
                    ss << numWithUnit->value << numWithUnit->unit;
                    finalValue = ss.str();
                } else if (const auto* str = std::get_if<std::string>(&evaluated.value)) {
                    finalValue = *str;
                } else {
                    // It's a boolean or something else that can't be a CSS value.
                    std::cerr << "Warning: Expression for property '" << key << "' evaluated to a non-CSS value." << std::endl;
                }
                if (!finalValue.empty()) {
                    node.processedStyles[key] = finalValue;
                }
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
