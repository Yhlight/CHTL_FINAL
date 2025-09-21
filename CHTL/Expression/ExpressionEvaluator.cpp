#include "ExpressionEvaluator.h"
#include "Expr.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleNode.h"
#include <iostream>
#include <cmath>
#include <map>
#include <sstream>
#include <algorithm>

namespace CHTL {

static bool isTruthy(const EvaluatedValue& val) {
    if (!val.unit.empty() && val.unit != "0") return true;
    return val.value != 0;
}

// --- Helper for findElement ---
// Recursive helper to collect all nodes matching a simple selector
void collectNodes(BaseNode* context, const std::string& simple_selector, std::vector<ElementNode*>& found_nodes) {
    if (!context) return;
    ElementNode* element = dynamic_cast<ElementNode*>(context);
    if (!element) return;

    char selector_type = simple_selector.empty() ? ' ' : simple_selector[0];
    std::string selector_value = simple_selector.substr(1);

    bool match = false;
    if (selector_type == '#') {
        for (const auto& attr : element->attributes) {
            if (attr.key == "id" && attr.value == selector_value) {
                match = true;
                break;
            }
        }
    } else if (selector_type == '.') {
        for (const auto& attr : element->attributes) {
            if (attr.key == "class") {
                // Whole word match
                std::stringstream ss(attr.value);
                std::string class_name;
                while (ss >> class_name) {
                    if (class_name == selector_value) {
                        match = true;
                        break;
                    }
                }
            }
            if (match) break;
        }
    } else {
        if (element->tagName == simple_selector) {
            match = true;
        }
    }

    if (match) {
        found_nodes.push_back(element);
    }

    for (const auto& child : element->children) {
        collectNodes(child.get(), simple_selector, found_nodes);
    }
}


ExpressionEvaluator::ExpressionEvaluator(const std::map<std::string, std::map<std::string, TemplateDefinitionNode>>& templates, BaseNode* doc_root)
    : templates(templates), doc_root(doc_root) {}

EvaluatedValue ExpressionEvaluator::evaluate(Expr* expr, ElementNode* context) {
    this->current_context = context;
    if (expr) {
        expr->accept(*this);
    }
    return result;
}

void ExpressionEvaluator::visit(LiteralExpr& expr) {
    if (expr.type == LiteralExpr::LiteralType::STRING) {
        // For string literals, the 'unit' holds the entire value.
        result = {0, expr.unit, EvaluatedType::STRING};
    } else {
        // For numbers, use the value and unit.
        result = {expr.value, expr.unit, EvaluatedType::NUMBER};
    }
}

void ExpressionEvaluator::visit(BinaryExpr& expr) {
    EvaluatedValue left = evaluate(expr.left.get(), this->current_context);
    EvaluatedValue right = evaluate(expr.right.get(), this->current_context);
    if (!left.unit.empty() && !right.unit.empty() && left.unit != right.unit) {
        throw std::runtime_error("Mismatched units in expression.");
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
    // If there's an override value, evaluate and return it immediately.
    if (expr.override_value) {
        result = evaluate(expr.override_value.get(), this->current_context);
        return;
    }

    // Otherwise, look up the variable in the template definitions.
    const TemplateDefinitionNode* var_group_def = nullptr;
    for (const auto& ns_pair : templates) {
        if (ns_pair.second.count(expr.group)) {
            var_group_def = &ns_pair.second.at(expr.group);
            break;
        }
    }

    if (var_group_def && var_group_def->type == TemplateType::VAR) {
        if (var_group_def->variables.count(expr.name)) {
            result = evaluate(var_group_def->variables.at(expr.name).get(), this->current_context);
            return;
        }
    }

    throw std::runtime_error("Variable not found: " + expr.group + "(" + expr.name + ")");
}

void ExpressionEvaluator::visit(ReferenceExpr& expr) {
    ElementNode* target_element = nullptr;
    if (expr.selector.lexeme.empty()) {
        // This is a self-reference to the current element's property
        target_element = this->current_context;
    } else {
        // This is a reference to another element
        target_element = findElement(doc_root, expr.selector.lexeme);
    }

    std::string full_ref_name = expr.selector.lexeme + "." + expr.property.lexeme;
    if (resolution_stack.count(full_ref_name)) {
        throw std::runtime_error("Circular property reference detected: " + full_ref_name);
    }

    if (!target_element) {
        throw std::runtime_error("Reference selector '" + expr.selector.lexeme + "' not found.");
    }

    // This logic is duplicated from the main generator, but is necessary here
    // to correctly calculate the property value of the referenced element.
    for (const auto& child : target_element->children) {
        if (StyleNode* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            std::map<std::string, AttributeNode> final_props;

            // 1. Apply templates
            for (const auto& app : styleNode->template_applications) {
                const TemplateDefinitionNode* def = nullptr;
                for (const auto& ns_pair : this->templates) {
                    if (ns_pair.second.count(app.template_name)) {
                        def = &ns_pair.second.at(app.template_name);
                        break;
                    }
                }
                if (def && def->type == TemplateType::STYLE) {
                    for (const auto& prop : def->style_properties) { final_props[prop.key] = prop.clone(); }
                    for (const auto& key_to_delete : app.deleted_properties) { final_props.erase(key_to_delete); }
                    for (const auto& prop : app.new_or_overridden_properties) { final_props[prop.key] = prop.clone(); }
                }
            }
            // 2. Apply direct properties
            for (const auto& prop : styleNode->direct_properties) {
                final_props[prop.key] = prop.clone();
            }

            // 3. Find and evaluate the requested property
            if (final_props.count(expr.property.lexeme)) {
                resolution_stack.insert(full_ref_name);
                result = evaluate(final_props.at(expr.property.lexeme).value_expr.get(), target_element);
                resolution_stack.erase(full_ref_name);
                return;
            }
        }
    }
    throw std::runtime_error("Reference property '" + expr.property.lexeme + "' not found on element '" + expr.selector.lexeme + "'.");
}

void ExpressionEvaluator::visit(ComparisonExpr& expr) {
    EvaluatedValue left = evaluate(expr.left.get(), this->current_context);
    EvaluatedValue right = evaluate(expr.right.get(), this->current_context);
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
    EvaluatedValue left = evaluate(expr.left.get(), this->current_context);
    if (expr.op.type == TokenType::PIPE_PIPE) {
        if (isTruthy(left)) { result = left; return; }
    } else {
        if (!isTruthy(left)) { result = left; return; }
    }
    result = evaluate(expr.right.get(), this->current_context);
}

void ExpressionEvaluator::visit(ConditionalExpr& expr) {
    EvaluatedValue condition = evaluate(expr.condition.get(), this->current_context);
    if (isTruthy(condition)) {
        result = evaluate(expr.then_branch.get(), this->current_context);
    } else {
        result = evaluate(expr.else_branch.get(), this->current_context);
    }
}

void ExpressionEvaluator::visit(DynamicReferenceExpr& expr) {
    // This should not be evaluated statically. The generator is responsible for
    // creating the dynamic JS code. If this is called, it's an error in the
    // generator's logic.
    throw std::runtime_error("DynamicReferenceExpr cannot be evaluated statically.");
}

ElementNode* ExpressionEvaluator::findElement(BaseNode* context, const std::string& selector) {
    // Handle indexed selectors like "div[0]"
    size_t bracket_pos = selector.find('[');
    std::string base_selector = selector;
    int index = 0;
    if (bracket_pos != std::string::npos) {
        base_selector = selector.substr(0, bracket_pos);
        size_t end_bracket_pos = selector.find(']', bracket_pos);
        try {
            index = std::stoi(selector.substr(bracket_pos + 1, end_bracket_pos - bracket_pos - 1));
        } catch (...) {
            throw std::runtime_error("Invalid index in selector: " + selector);
        }
    }

    // Handle descendant selectors
    std::stringstream ss(base_selector);
    std::string segment;
    std::vector<std::string> segments;
    while(ss >> segment) {
        segments.push_back(segment);
    }

    std::vector<ElementNode*> current_nodes;
    collectNodes(context, segments[0], current_nodes);

    for (size_t i = 1; i < segments.size(); ++i) {
        std::vector<ElementNode*> next_nodes;
        for (ElementNode* node : current_nodes) {
            collectNodes(node, segments[i], next_nodes);
        }
        current_nodes = next_nodes;
    }

    if (index < current_nodes.size()) {
        return current_nodes[index];
    }

    return nullptr;
}

} // namespace CHTL
