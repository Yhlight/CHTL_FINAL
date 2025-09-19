#include "ExpressionEvaluator.h"
#include "Expr.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../../Util/StringUtil/StringUtil.h"
#include "../Util/StyleResolver.h"
#include <iostream>
#include <cmath>
#include <map>
#include <vector>

namespace CHTL {

static bool matchesSelector(ElementNode* element, const std::string& selector);
static ElementNode* findDescendant(BaseNode* context, const std::string& selector);

static bool isTruthy(const EvaluatedValue& val) {
    // A value is "truthy" if it's a non-zero number or a non-empty string.
    // A string literal is represented with value=0 and unit=the_string.
    if (val.value != 0) return true;
    if (!val.unit.empty() && val.unit != "0") return true;
    return false;
}

ExpressionEvaluator::ExpressionEvaluator(
    const std::map<std::string, AttributeNode>* style_context,
    const std::map<std::string, std::map<std::string, TemplateDefinitionNode>>& templates,
    BaseNode* doc_root)
    : style_context(style_context), templates(templates), doc_root(doc_root) {}

EvaluatedValue ExpressionEvaluator::evaluate(Expr* expr, ElementNode* context_element) {
    this->current_element_context = context_element;
    if (expr) {
        expr->accept(*this);
    }
    return result;
}

void ExpressionEvaluator::visit(LiteralExpr& expr) {
    if (expr.unit.empty() || expr.value != 0) {
         result = {expr.value, expr.unit};
    } else {
        result = {0, expr.unit};
    }
}

void ExpressionEvaluator::visit(BinaryExpr& expr) {
    EvaluatedValue left = evaluate(expr.left.get(), this->current_element_context);
    EvaluatedValue right = evaluate(expr.right.get(), this->current_element_context);

    // Basic string concatenation
    if (expr.op.type == TokenType::PLUS && (left.value == 0 || right.value == 0)) {
        result = {0, left.unit + right.unit};
        return;
    }

    if (!left.unit.empty() && !right.unit.empty() && left.unit != right.unit) {
        // Allow operations with 0, treating it as a universal unit
        if (left.value != 0 && right.value != 0) {
            throw std::runtime_error("Mismatched units in expression.");
        }
    }
    std::string result_unit = !left.unit.empty() ? left.unit : right.unit;
    double result_value = 0.0;
    switch (expr.op.type) {
        case TokenType::PLUS: result_value = left.value + right.value; break;
        case TokenType::MINUS: result_value = left.value - right.value; break;
        case TokenType::STAR: result_value = left.value * right.value; break;
        case TokenType::SLASH: result_value = right.value == 0 ? 0 : left.value / right.value; break;
        case TokenType::PERCENT: result_value = fmod(left.value, right.value); break;
        case TokenType::STAR_STAR: result_value = pow(left.value, right.value); break;
        default: throw std::runtime_error("Unknown binary operator.");
    }
    result = {result_value, result_unit};
}

void ExpressionEvaluator::visit(VarExpr& expr) {
    // This is a simplified implementation
    if (templates.count(expr.group) && templates.at(expr.group).count(expr.name)) {
        const auto& var_def = templates.at(expr.group).at(expr.name);
        if (var_def.variables.count(expr.name)) {
            result = evaluate(var_def.variables.at(expr.name).get(), this->current_element_context);
            return;
        }
    }
    throw std::runtime_error("Variable not found: " + expr.group + "." + expr.name);
}

void ExpressionEvaluator::visit(ReferenceExpr& expr) {
    std::stringstream unique_prop_key;
    ElementNode* target_element_for_eval = nullptr;

    // Determine the target element and construct a unique key for cycle detection
    if (expr.selector.lexeme.empty()) {
        // Self-reference
        target_element_for_eval = this->current_element_context;
        unique_prop_key << static_cast<const void*>(target_element_for_eval) << "." << expr.property.lexeme;
    } else {
        // External reference
        target_element_for_eval = findElement(doc_root, expr.selector.lexeme);
        if (!target_element_for_eval) {
            throw std::runtime_error("Reference selector '" + expr.selector.lexeme + "' not found.");
        }
        unique_prop_key << static_cast<const void*>(target_element_for_eval) << "." << expr.property.lexeme;
    }

    // Check for circular dependencies
    if (resolution_stack.count(unique_prop_key.str())) {
        throw std::runtime_error("Circular property reference detected: " + unique_prop_key.str());
    }
    resolution_stack.insert(unique_prop_key.str());

    // --- Resolve and Evaluate ---
    // Note: The logic for self-reference vs external reference is now unified.
    // We always resolve the styles for the target element and then evaluate.

    StyleNode* target_style_node = nullptr;
    for (const auto& child : target_element_for_eval->children) {
        if (StyleNode* sn = dynamic_cast<StyleNode*>(child.get())) {
            target_style_node = sn;
            break;
        }
    }

    if (target_style_node) {
        std::map<std::string, AttributeNode> target_final_props;
        StyleResolver::resolveStyleNode(target_style_node, this->templates, target_final_props);

        if (target_final_props.count(expr.property.lexeme)) {
            // Use the target's props as the context for a new evaluation.
            ExpressionEvaluator temp_eval(&target_final_props, this->templates, this->doc_root);
            result = temp_eval.evaluate(target_final_props.at(expr.property.lexeme).value_expr.get(), target_element_for_eval);

            resolution_stack.erase(unique_prop_key.str());
            return;
        }
    }

    resolution_stack.erase(unique_prop_key.str()); // Clean up stack on failure
    throw std::runtime_error("Referenced property '" + expr.property.lexeme + "' not found on element.");
}

void ExpressionEvaluator::visit(BoolExpr& expr) {
    result = {expr.value ? 1.0 : 0.0, ""};
}

void ExpressionEvaluator::visit(ComparisonExpr& expr) {
    EvaluatedValue left = evaluate(expr.left.get(), this->current_element_context);
    EvaluatedValue right = evaluate(expr.right.get(), this->current_element_context);
    bool comparison_result = false;
    switch (expr.op.type) {
        case TokenType::GREATER:       comparison_result = left.value > right.value; break;
        case TokenType::GREATER_EQUAL: comparison_result = left.value >= right.value; break;
        case TokenType::LESS:          comparison_result = left.value < right.value; break;
        case TokenType::LESS_EQUAL:    comparison_result = left.value <= right.value; break;
        case TokenType::EQUAL_EQUAL:   comparison_result = left.value == right.value && left.unit == right.unit; break;
        case TokenType::BANG_EQUAL:    comparison_result = left.value != right.value || left.unit != right.unit; break;
        default: break;
    }
    result = {comparison_result ? 1.0 : 0.0, ""};
}

void ExpressionEvaluator::visit(LogicalExpr& expr) {
    EvaluatedValue left = evaluate(expr.left.get(), this->current_element_context);
    if (expr.op.type == TokenType::PIPE_PIPE) {
        if (isTruthy(left)) { result = left; return; }
    } else {
        if (!isTruthy(left)) { result = left; return; }
    }
    result = evaluate(expr.right.get(), this->current_element_context);
}

void ExpressionEvaluator::visit(ConditionalExpr& expr) {
    EvaluatedValue condition = evaluate(expr.condition.get(), this->current_element_context);
    if (isTruthy(condition)) {
        result = evaluate(expr.then_branch.get(), this->current_element_context);
    } else {
        result = evaluate(expr.else_branch.get(), this->current_element_context);
    }
}

// Forward declaration for the recursive helper
static void findDescendantsRecursive(BaseNode* context, const std::string& selector, std::vector<ElementNode*>& found_nodes);

ElementNode* ExpressionEvaluator::findElement(BaseNode* context, const std::string& selector) {
    std::vector<std::string> parts = StringUtil::split(selector, ' ');
    if (parts.empty()) {
        return nullptr;
    }

    std::vector<ElementNode*> current_matches;
    ElementNode* start_element = dynamic_cast<ElementNode*>(context);

    // Handle the first part of the selector. It can match the context itself or any of its descendants.
    if (start_element && matchesSelector(start_element, parts[0])) {
        current_matches.push_back(start_element);
    }
    findDescendantsRecursive(context, parts[0], current_matches);


    for (size_t i = 1; i < parts.size(); ++i) {
        if (current_matches.empty()) {
            return nullptr;
        }
        std::vector<ElementNode*> next_matches;
        for (ElementNode* match : current_matches) {
            findDescendantsRecursive(match, parts[i], next_matches);
        }
        current_matches = next_matches;
    }

    return current_matches.empty() ? nullptr : current_matches[0];
}


static bool matchesSelector(ElementNode* element, const std::string& selector) {
    if (!element || selector.empty()) {
        return false;
    }

    char selector_type = selector[0];

    if (selector_type == '#') {
        std::string selector_value = selector.substr(1);
        for (const auto& attr : element->attributes) {
            if (attr.key == "id" && attr.value == selector_value) {
                return true;
            }
        }
    } else if (selector_type == '.') {
        std::string selector_value = selector.substr(1);
        for (const auto& attr : element->attributes) {
            if (attr.key == "class") {
                std::vector<std::string> classes = StringUtil::split(attr.value, ' ');
                for (const auto& cls : classes) {
                    if (cls == selector_value) {
                        return true;
                    }
                }
            }
        }
    } else {
        if (element->tagName == selector) {
            return true;
        }
    }
    return false;
}

static void findDescendantsRecursive(BaseNode* context, const std::string& selector, std::vector<ElementNode*>& found_nodes) {
    ElementNode* element = dynamic_cast<ElementNode*>(context);
    if (!element) {
        return;
    }

    for (const auto& child : element->children) {
        ElementNode* child_element = dynamic_cast<ElementNode*>(child.get());
        if (child_element) {
            if (matchesSelector(child_element, selector)) {
                // To avoid duplicates if a node was already matched (e.g. as the start_element)
                bool already_found = false;
                for(const auto& node : found_nodes) {
                    if (node == child_element) {
                        already_found = true;
                        break;
                    }
                }
                if (!already_found) {
                    found_nodes.push_back(child_element);
                }
            }
            findDescendantsRecursive(child_element, selector, found_nodes);
        }
    }
}


} // namespace CHTL
