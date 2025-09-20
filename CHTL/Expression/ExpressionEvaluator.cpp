#include "ExpressionEvaluator.h"
#include "../CHTLNode/ReactiveValueNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../../Util/StringUtil/StringUtil.h" // For splitting class names
#include <algorithm> // For std::any_of
#include <iostream>
#include <cmath>

namespace CHTL {

// Helper to check for truthiness
static bool isTruthy(const EvaluatedValue& val) {
    if (val.type == ValueType::BOOL) return val.numeric_value != 0;
    if (val.type == ValueType::STRING) return !val.string_value.empty() && val.string_value != "0";
    return val.numeric_value != 0;
}

ExpressionEvaluator::ExpressionEvaluator(const std::map<std::string, TemplateDefinitionNode>& templates, BaseNode* doc_root)
    : templates(templates), doc_root(doc_root) {}

EvaluatedValue ExpressionEvaluator::evaluate(Expr* expr, ElementNode* context) {
    this->current_context = context;
    if (expr) {
        expr->accept(*this);
    }
    return result;
}

void ExpressionEvaluator::visit(LiteralExpr& expr) {
    if (expr.type == LiteralType::NUMERIC) {
        result = {ValueType::NUMERIC, expr.numeric_value, expr.string_value};
    } else { // STRING
        result = {ValueType::STRING, 0, expr.string_value};
    }
}

void ExpressionEvaluator::visit(BinaryExpr& expr) {
    EvaluatedValue left = evaluate(expr.left.get(), this->current_context);
    EvaluatedValue right = evaluate(expr.right.get(), this->current_context);

    if (left.type != ValueType::NUMERIC || right.type != ValueType::NUMERIC) {
        throw std::runtime_error("Arithmetic operations can only be performed on numeric values.");
    }

    if (!left.string_value.empty() && !right.string_value.empty() && left.string_value != right.string_value) {
        throw std::runtime_error("Mismatched units in expression: '" + left.string_value + "' and '" + right.string_value + "'.");
    }

    std::string result_unit = !left.string_value.empty() ? left.string_value : right.string_value;
    double result_value = 0.0;

    switch (expr.op.type) {
        case TokenType::PLUS: result_value = left.numeric_value + right.numeric_value; break;
        case TokenType::MINUS: result_value = left.numeric_value - right.numeric_value; break;
        case TokenType::STAR: result_value = left.numeric_value * right.numeric_value; break;
        case TokenType::SLASH:
            if (right.numeric_value == 0) throw std::runtime_error("Division by zero.");
            result_value = left.numeric_value / right.numeric_value;
            break;
        case TokenType::PERCENT:
            if (static_cast<int>(right.numeric_value) == 0) throw std::runtime_error("Modulo by zero.");
            result_value = fmod(left.numeric_value, right.numeric_value);
            break;
        case TokenType::STAR_STAR: result_value = pow(left.numeric_value, right.numeric_value); break;
        default: throw std::runtime_error("Unknown binary operator.");
    }

    result = {ValueType::NUMERIC, result_value, result_unit};
}

void ExpressionEvaluator::visit(VarExpr& expr) {
    std::string full_var_name = expr.group + "." + expr.name;
    if (resolution_stack.count(full_var_name)) {
        throw std::runtime_error("Circular variable reference detected for: " + full_var_name);
    }

    if (templates.count(expr.group)) {
        const auto& template_def = templates.at(expr.group);
        if (template_def.type == TemplateType::VAR && template_def.variables.count(expr.name)) {
            resolution_stack.insert(full_var_name);
            result = evaluate(template_def.variables.at(expr.name).get(), this->current_context);
            resolution_stack.erase(full_var_name);
            return;
        }
    }
    throw std::runtime_error("Variable not found: " + full_var_name);
}

void ExpressionEvaluator::visit(ReferenceExpr& expr) {
    std::string full_ref_name = expr.selector.lexeme + "." + expr.property.lexeme;
    if (resolution_stack.count(full_ref_name)) {
        throw std::runtime_error("Circular property reference detected for: " + full_ref_name);
    }

    ElementNode* target_element = nullptr;
    if (expr.selector.lexeme.empty()) { // Implicit self-reference
        target_element = this->current_context;
    } else {
        target_element = findElement(doc_root, expr.selector.lexeme);
    }

    if (!target_element) {
        throw std::runtime_error("Reference error: selector '" + expr.selector.lexeme + "' not found.");
    }

    for (const auto& child : target_element->children) {
        if (StyleNode* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            for (const auto& prop : styleNode->inline_properties) {
                if (prop.key == expr.property.lexeme) {
                    resolution_stack.insert(full_ref_name);
                    result = evaluate(prop.value_expr.get(), target_element); // Evaluate in the context of the target
                    resolution_stack.erase(full_ref_name);
                    return;
                }
            }
        }
    }

    throw std::runtime_error("Reference error: property '" + expr.property.lexeme + "' not found on element with selector '" + expr.selector.lexeme + "'.");
}

void ExpressionEvaluator::visit(ComparisonExpr& expr) {
    EvaluatedValue left = evaluate(expr.left.get(), this->current_context);
    EvaluatedValue right = evaluate(expr.right.get(), this->current_context);
    bool comparison_result = false;

    if (left.type != right.type) {
        // For now, only compare values of the same type.
        // Could be extended to allow cross-type comparisons.
        comparison_result = false;
    } else if (left.type == ValueType::NUMERIC) {
        switch (expr.op.type) {
            case TokenType::GREATER:       comparison_result = left.numeric_value > right.numeric_value; break;
            case TokenType::GREATER_EQUAL: comparison_result = left.numeric_value >= right.numeric_value; break;
            case TokenType::LESS:          comparison_result = left.numeric_value < right.numeric_value; break;
            case TokenType::LESS_EQUAL:    comparison_result = left.numeric_value <= right.numeric_value; break;
            case TokenType::EQUAL_EQUAL:   comparison_result = left.numeric_value == right.numeric_value && left.string_value == right.string_value; break;
            case TokenType::BANG_EQUAL:    comparison_result = left.numeric_value != right.numeric_value || left.string_value != right.string_value; break;
            default: break;
        }
    } else { // STRING
        switch (expr.op.type) {
            case TokenType::EQUAL_EQUAL:   comparison_result = left.string_value == right.string_value; break;
            case TokenType::BANG_EQUAL:    comparison_result = left.string_value != right.string_value; break;
            default: // Other comparisons are not well-defined for strings here.
                comparison_result = false;
                break;
        }
    }

    result = {ValueType::BOOL, comparison_result ? 1.0 : 0.0, ""};
}

void ExpressionEvaluator::visit(LogicalExpr& expr) {
    EvaluatedValue left = evaluate(expr.left.get(), this->current_context);

    if (expr.op.type == TokenType::PIPE_PIPE) {
        if (isTruthy(left)) {
            result = {ValueType::BOOL, 1.0, ""};
            return;
        }
    } else { // AMPERSAND_AMPERSAND
        if (!isTruthy(left)) {
            result = {ValueType::BOOL, 0.0, ""};
            return;
        }
    }

    EvaluatedValue right = evaluate(expr.right.get(), this->current_context);
    result = {ValueType::BOOL, isTruthy(right) ? 1.0 : 0.0, ""};
}

void ExpressionEvaluator::visit(ConditionalExpr& expr) {
    EvaluatedValue condition = evaluate(expr.condition.get(), this->current_context);
    if (isTruthy(condition)) {
        result = evaluate(expr.then_branch.get(), this->current_context);
    } else {
        result = evaluate(expr.else_branch.get(), this->current_context);
    }
}

// Helper function to find all elements matching a tag name
void findElementsRecursive(BaseNode* context, const std::string& tag_name, std::vector<ElementNode*>& found_elements) {
    if (!context) return;
    ElementNode* element = dynamic_cast<ElementNode*>(context);
    if (!element) return;

    if (element->tagName == tag_name) {
        found_elements.push_back(element);
    }

    for (const auto& child : element->children) {
        findElementsRecursive(child.get(), tag_name, found_elements);
    }
}


ElementNode* ExpressionEvaluator::findElement(BaseNode* context, const std::string& selector) {
    // Handle tag[index] selector
    size_t bracket_pos = selector.find('[');
    if (bracket_pos != std::string::npos) {
        std::string tag_name = selector.substr(0, bracket_pos);
        size_t end_bracket_pos = selector.find(']');
        if (end_bracket_pos == std::string::npos) {
            throw std::runtime_error("Invalid selector format: missing ']' in " + selector);
        }
        std::string index_str = selector.substr(bracket_pos + 1, end_bracket_pos - bracket_pos - 1);
        int index = std::stoi(index_str);

        std::vector<ElementNode*> all_matching_elements;
        findElementsRecursive(this->doc_root, tag_name, all_matching_elements);

        if (index >= 0 && index < all_matching_elements.size()) {
            return all_matching_elements[index];
        }
        return nullptr; // Index out of bounds
    }

    // Original logic for id, class, and simple tag selectors (non-recursive part)
    std::vector<BaseNode*> nodes_to_search;
    nodes_to_search.push_back(this->doc_root);

    while(!nodes_to_search.empty()) {
        BaseNode* current_node = nodes_to_search.back();
        nodes_to_search.pop_back();

        ElementNode* element = dynamic_cast<ElementNode*>(current_node);
        if (!element) continue;

        char selector_type = selector.empty() ? ' ' : selector[0];
        bool found = false;
        if (selector_type == '#') {
            std::string id = selector.substr(1);
            for (const auto& attr : element->attributes) {
                if (attr.key == "id" && attr.value == id) found = true;
            }
        } else if (selector_type == '.') {
            std::string className = selector.substr(1);
            for (const auto& attr : element->attributes) {
                if (attr.key == "class") {
                    std::vector<std::string> classes = StringUtil::split(attr.value, ' ');
                    if (std::any_of(classes.begin(), classes.end(),
                        [&](const std::string& c){ return c == className; })) {
                        found = true;
                    }
                }
            }
        } else { // Simple tag selector
            if (element->tagName == selector) found = true;
        }

        if (found) return element;

        // Add children to the search stack (in reverse to maintain document order)
        for (auto it = element->children.rbegin(); it != element->children.rend(); ++it) {
            nodes_to_search.push_back(it->get());
        }
    }

    return nullptr; // Not found
}

void ExpressionEvaluator::visit(ReactiveValueNode& expr) {
    // For now, the evaluator doesn't do anything with reactive values.
    // The generator will handle them specially.
    // We can assign a placeholder value.
    result = {ValueType::STRING, 0, "$(" + expr.variable_name + ")"};
}

} // namespace CHTL
