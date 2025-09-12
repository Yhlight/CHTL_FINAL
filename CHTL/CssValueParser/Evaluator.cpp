#include "Evaluator.h"
#include "../CHTLNode/ElementNode.h"
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <sstream>

// Helper to find a node by ID
std::shared_ptr<ElementNode> findNodeById(std::shared_ptr<BaseNode> currentNode, const std::string& id) {
    if (!currentNode) return nullptr;
    if (auto element = std::dynamic_pointer_cast<ElementNode>(currentNode)) {
        if (element->attributes.count("id") && element->attributes["id"] == id) {
            return element;
        }
        for (const auto& child : element->children) {
            auto found = findNodeById(child, id);
            if (found) return found;
        }
    }
    return nullptr;
}

template<typename T, typename... Ts>
bool is(const std::variant<Ts...>& v) {
    return std::holds_alternative<T>(v);
}

EvaluatedValue Evaluator::evaluate(std::shared_ptr<ExprNode> expr, std::shared_ptr<BaseNode> ChtlAstRoot, ElementNode* current, std::set<std::string>& stack) {
    if (expr) {
        this->chtlAstRoot = ChtlAstRoot;
        this->currentElement = current;
        this->evaluationStack = &stack;
        expr->accept(*this);
    } else {
        result = {{}, true, "Invalid expression tree."};
    }
    return result;
}

void Evaluator::visit(NumericLiteralNode& node) {
    result = {{NumberWithUnit{node.value, node.unit}}, false, ""};
}

void Evaluator::visit(StringLiteralNode& node) {
    result = {{node.value}, false, ""};
}

void Evaluator::visit(PropertyReferenceNode& node) {
    ElementNode* targetNode = nullptr;
    if (node.selector.empty()) { // Implicit reference to self
        targetNode = this->currentElement;
    } else { // Explicit reference to another element
        if (node.selector.rfind('#', 0) == 0) {
            targetNode = findNodeById(this->chtlAstRoot, node.selector.substr(1)).get();
        } else {
            result = {{}, true, "Only ID selectors are currently supported for property references."}; return;
        }
    }

    if (!targetNode) {
        result = {{}, true, "Could not find element with selector: " + node.selector}; return;
    }

    // Check for circular dependencies
    std::string propertyIdentifier = (node.selector.empty() ? "self" : node.selector) + "." + node.propertyName;
    if (this->evaluationStack->count(propertyIdentifier)) {
        result = {{}, true, "Circular dependency detected: " + propertyIdentifier}; return;
    }
    this->evaluationStack->insert(propertyIdentifier);

    if (targetNode->unevaluatedStyles.count(node.propertyName)) {
        result = evaluate(targetNode->unevaluatedStyles[node.propertyName], this->chtlAstRoot, targetNode, *this->evaluationStack);
    } else {
        result = {{}, true, "Property '" + node.propertyName + "' not found on element '" + node.selector + "'."};
    }

    this->evaluationStack->erase(propertyIdentifier); // Pop from stack
}

void Evaluator::visit(ConditionalExprNode& node) {
    EvaluatedValue conditionResult = evaluate(node.condition, this->chtlAstRoot, this->currentElement, *this->evaluationStack);
    if (conditionResult.hasError) { result = conditionResult; return; }
    if (!is<bool>(conditionResult.value)) { result = {{}, true, "Condition did not evaluate to a boolean."}; return; }

    if (std::get<bool>(conditionResult.value)) {
        result = evaluate(node.trueBranch, this->chtlAstRoot, this->currentElement, *this->evaluationStack);
    } else {
        result = evaluate(node.falseBranch, this->chtlAstRoot, this->currentElement, *this->evaluationStack);
    }
}

void Evaluator::visit(BinaryOpNode& node) {
    EvaluatedValue left = evaluate(node.left, this->chtlAstRoot, this->currentElement, *this->evaluationStack);
    if (left.hasError) { result = left; return; }
    EvaluatedValue right = evaluate(node.right, this->chtlAstRoot, this->currentElement, *this->evaluationStack);
    if (right.hasError) { result = right; return; }

    if (node.op.type >= ValueTokenType::TOKEN_LOGICAL_OR && node.op.type <= ValueTokenType::TOKEN_LOGICAL_AND) {
        if (!is<bool>(left.value) || !is<bool>(right.value)) { result = {{}, true, "Logical operators require boolean operands."}; return; }
        bool l = std::get<bool>(left.value);
        bool r = std::get<bool>(right.value);
        result = {{node.op.type == ValueTokenType::TOKEN_LOGICAL_AND ? (l && r) : (l || r)}, false, ""};
        return;
    }

    if (is<NumberWithUnit>(left.value) && is<NumberWithUnit>(right.value)) {
        NumberWithUnit l = std::get<NumberWithUnit>(left.value);
        NumberWithUnit r = std::get<NumberWithUnit>(right.value);
        if (node.op.type >= ValueTokenType::TOKEN_GREATER && node.op.type <= ValueTokenType::TOKEN_NOT_EQUAL) {
            if (l.unit != r.unit) { result = {{}, true, "Cannot compare numbers with different units."}; return; }
            bool comparisonResult = false;
            switch (node.op.type) {
                case ValueTokenType::TOKEN_GREATER: comparisonResult = l.value > r.value; break;
                case ValueTokenType::TOKEN_GREATER_EQUAL: comparisonResult = l.value >= r.value; break;
                case ValueTokenType::TOKEN_LESS: comparisonResult = l.value < r.value; break;
                case ValueTokenType::TOKEN_LESS_EQUAL: comparisonResult = l.value <= r.value; break;
                case ValueTokenType::TOKEN_EQUAL_EQUAL: comparisonResult = l.value == r.value; break;
                case ValueTokenType::TOKEN_NOT_EQUAL: comparisonResult = l.value != r.value; break;
                default: break;
            }
            result = {{comparisonResult}, false, ""};
            return;
        }

        double newValue = 0;
        std::string newUnit = l.unit;
        switch (node.op.type) {
            case ValueTokenType::TOKEN_PLUS:
            case ValueTokenType::TOKEN_MINUS:
                if (l.unit != r.unit) { result = {{}, true, "Mismatched units for + or -."}; return; }
                newValue = (node.op.type == ValueTokenType::TOKEN_PLUS) ? (l.value + r.value) : (l.value - r.value);
                break;
            case ValueTokenType::TOKEN_STAR:
                if (!l.unit.empty() && !r.unit.empty()) { result = {{}, true, "Cannot multiply two values with units."}; return; }
                newValue = l.value * r.value;
                if (!r.unit.empty()) newUnit = r.unit;
                break;
            case ValueTokenType::TOKEN_SLASH:
                if (!r.unit.empty()) { result = {{}, true, "Cannot divide by a value with units."}; return; }
                if (r.value == 0) { result = {{}, true, "Division by zero."}; return; }
                newValue = l.value / r.value;
                break;
            case ValueTokenType::TOKEN_POWER:
                if (!r.unit.empty()) { result = {{}, true, "Exponent must be unitless."}; return; }
                newValue = pow(l.value, r.value);
                break;
            default:
                result = {{}, true, "Unsupported binary operator."}; return;
        }
        result = {{NumberWithUnit{newValue, newUnit}}, false, ""};
        return;
    }

    result = {{}, true, "Invalid operands for binary operator."};
}
