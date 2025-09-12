#include "Evaluator.h"
#include "../CHTLNode/ElementNode.h" // Need to inspect ElementNodes
#include <cmath> // For pow()
#include <stdexcept>
#include <iostream>

// Helper function to recursively search the CHTL AST for a node with a given ID
std::shared_ptr<ElementNode> findNodeById(std::shared_ptr<BaseNode> currentNode, const std::string& id) {
    if (!currentNode) return nullptr;

    if (auto element = std::dynamic_pointer_cast<ElementNode>(currentNode)) {
        if (element->attributes.count("id") && element->attributes["id"] == id) {
            return element;
        }
    }

    // This is a simple implementation. A real one would need to check children.
    // For now, we assume a flat structure for simplicity.
    if (auto element = std::dynamic_pointer_cast<ElementNode>(currentNode)) {
        for (const auto& child : element->children) {
            auto found = findNodeById(child, id);
            if (found) return found;
        }
    }

    return nullptr;
}


EvaluatedValue Evaluator::evaluate(std::shared_ptr<ExprNode> expr, std::shared_ptr<BaseNode> ChtlAstRoot) {
    if (expr) {
        this->chtlAstRoot = ChtlAstRoot;
        expr->accept(*this);
    } else {
        result = {0, "", true, "Invalid expression tree."};
    }
    return result;
}

void Evaluator::visit(NumericLiteralNode& node) {
    result = {node.value, node.unit, false, ""};
}

void Evaluator::visit(PropertyReferenceNode& node) {
    if (!chtlAstRoot) {
        result = {0, "", true, "Cannot resolve property reference: CHTL AST not available."};
        return;
    }

    std::string selector = node.selector;
    if (selector.rfind('#', 0) != 0) {
        result = {0, "", true, "Cannot resolve property reference: Only ID selectors are currently supported."};
        return;
    }

    std::string idToFind = selector.substr(1);
    auto targetNode = findNodeById(chtlAstRoot, idToFind);

    if (!targetNode) {
        result = {0, "", true, "Could not find element with selector: " + selector};
        return;
    }

    // Found the node, now find the property
    std::string propertyName = node.propertyName;
    if (targetNode->unevaluatedStyles.count(propertyName)) {
        // The property is another expression, we need to evaluate it recursively.
        // This creates a dependency. We need to detect circular dependencies.
        // For now, we'll assume no circular dependencies for simplicity.
        Evaluator innerEvaluator;
        result = innerEvaluator.evaluate(targetNode->unevaluatedStyles[propertyName], this->chtlAstRoot);
    } else {
        result = {0, "", true, "Property '" + propertyName + "' not found on element '" + selector + "'."};
    }
}


void Evaluator::visit(BinaryOpNode& node) {
    // Note: The recursive calls to evaluate here will correctly propagate the ChtlAstRoot.
    EvaluatedValue left = evaluate(node.left, this->chtlAstRoot);
    if (left.hasError) {
        result = left;
        return;
    }

    EvaluatedValue right = evaluate(node.right, this->chtlAstRoot);
    if (right.hasError) {
        result = right;
        return;
    }

    // ... (rest of the BinaryOpNode logic is the same)
    double newValue = 0;
    std::string newUnit = left.unit;

    switch (node.op.type) {
        case ValueTokenType::TOKEN_PLUS:
        case ValueTokenType::TOKEN_MINUS:
            if (left.unit != right.unit) {
                result = {0, "", true, "Mismatched units for + or - operation."};
                return;
            }
            newValue = (node.op.type == ValueTokenType::TOKEN_PLUS) ? (left.value + right.value) : (left.value - right.value);
            break;

        case ValueTokenType::TOKEN_STAR:
            if (!left.unit.empty() && !right.unit.empty()) {
                result = {0, "", true, "Cannot multiply two values with units."};
                return;
            }
            newValue = left.value * right.value;
            if (right.unit.empty()) newUnit = left.unit;
            else newUnit = right.unit;
            break;

        case ValueTokenType::TOKEN_SLASH:
             if (!right.unit.empty()) {
                result = {0, "", true, "Cannot divide by a value with units."};
                return;
            }
            if (right.value == 0) {
                 result = {0, "", true, "Division by zero."};
                 return;
            }
            newValue = left.value / right.value;
            break;

        case ValueTokenType::TOKEN_POWER:
            if (!right.unit.empty()) {
                result = {0, "", true, "Exponent must be unitless."};
                return;
            }
            newValue = pow(left.value, right.value);
            break;

        default:
            result = {0, "", true, "Unsupported binary operator."};
            return;
    }

    result = {newValue, newUnit, false, ""};
}
