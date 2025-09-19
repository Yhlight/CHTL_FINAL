#include "ExpressionEvaluator.h"
#include "Expr.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLGenerator/CHTLGenerator.h" // Include for computeFinalStyles
#include <iostream>
#include <cmath>
#include <stdexcept>
#include <sstream>
#include <algorithm>

namespace CHTL {

// --- Helper function to find all nodes matching a simple selector ---
void findElementsRecursive(BaseNode* context, const std::string& selector, std::vector<ElementNode*>& found_elements) {
    if (!context) return;

    // We only search within ElementNodes
    ElementNode* element = dynamic_cast<ElementNode*>(context);
    if (!element) {
        // If context is not an element, it can't match, but its children might.
        // This case is handled by the loop below if the context is the root.
        // But for now, we assume we start with an element or a node containing elements.
    }

    if (element) {
        char selector_type = selector.empty() ? ' ' : selector[0];
        bool match = false;
        if (selector_type == '#') {
            std::string id = selector.substr(1);
            for (const auto& attr : element->attributes) {
                if (attr.key == "id") {
                    if (auto* literal = dynamic_cast<LiteralExpr*>(attr.value_expr.get())) {
                        if (literal->is_string && literal->string_value == id) {
                            match = true;
                            break;
                        }
                    }
                }
            }
        } else if (selector_type == '.') {
            std::string className = selector.substr(1);
            for (const auto& attr : element->attributes) {
                if (attr.key == "class") {
                    if (auto* literal = dynamic_cast<LiteralExpr*>(attr.value_expr.get())) {
                        if (literal->is_string) {
                            std::stringstream ss(literal->string_value);
                            std::string item;
                            while (std::getline(ss, item, ' ')) {
                                if (item == className) {
                                    match = true;
                                    break;
                                }
                            }
                        }
                    }
                }
                if (match) break;
            }
        } else { // Tag selector
            if (element->tagName == selector) {
                match = true;
            }
        }

        if (match) {
            found_elements.push_back(element);
        }
    }

    // Determine the children to recurse into
    std::vector<std::unique_ptr<BaseNode>>* children_ptr = nullptr;
    if(element) {
        children_ptr = &element->children;
    } else {
        // This could be the root node which might not be an element itself
        // but contains the main element (e.g. a DocumentNode).
        // For now, we assume root is an element or has element children.
        // A proper DocumentNode would be better.
    }

    if(element && !element->children.empty()){
        for (const auto& child : element->children) {
            findElementsRecursive(child.get(), selector, found_elements);
        }
    }
}


static bool isTruthy(const PropertyValue& val) {
    if (val.is_string) {
        return !val.string_value.empty();
    }
    return val.numeric_value != 0;
}

ExpressionEvaluator::ExpressionEvaluator(const std::map<std::string, std::map<std::string, TemplateDefinitionNode>>& templates, BaseNode* doc_root)
    : templates(templates), doc_root(doc_root) {}

PropertyValue ExpressionEvaluator::evaluate(Expr* expr, ElementNode* context) {
    this->current_context = context;
    if (expr) {
        expr->accept(*this);
    }
    return result;
}

void ExpressionEvaluator::visit(LiteralExpr& expr) {
    if (expr.is_string) {
        result = {true, 0.0, "", expr.string_value};
    } else {
        result = {false, expr.numeric_value, expr.unit, ""};
    }
}

void ExpressionEvaluator::visit(BinaryExpr& expr) {
    PropertyValue left = evaluate(expr.left.get(), this->current_context);
    PropertyValue right = evaluate(expr.right.get(), this->current_context);

    // Handle string concatenation for '+' operator
    if (expr.op.type == TokenType::PLUS) {
        if (left.is_string || right.is_string) {
            result = {true, 0.0, "", left.toString() + right.toString()};
            return;
        }
    }

    // For other operators, or for two numeric operands with +, ensure both are numeric.
    if (left.is_string || right.is_string) {
        throw std::runtime_error("Binary operator " + expr.op.lexeme + " cannot be applied to string values.");
    }

    if (!left.unit.empty() && !right.unit.empty() && left.unit != right.unit) {
        throw std::runtime_error("Mismatched units in expression.");
    }
    std::string result_unit = !left.unit.empty() ? left.unit : right.unit;
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
            if (right.numeric_value == 0) throw std::runtime_error("Modulo by zero.");
            result_value = fmod(left.numeric_value, right.numeric_value);
            break;
        case TokenType::STAR_STAR: result_value = pow(left.numeric_value, right.numeric_value); break;
        default: throw std::runtime_error("Unknown binary operator for numeric values.");
    }
    result = {false, result_value, result_unit, ""};
}

void ExpressionEvaluator::visit(VarExpr& expr) {
    // Look for the variable group in all available namespaces
    const TemplateDefinitionNode* var_group_def = nullptr;
    for (const auto& ns_pair : this->templates) {
        if (ns_pair.second.count(expr.group)) {
            const auto& potential_def = ns_pair.second.at(expr.group);
            if (potential_def.type == TemplateType::VAR) {
                var_group_def = &potential_def;
                break;
            }
        }
    }

    if (!var_group_def) {
        throw std::runtime_error("Variable group '" + expr.group + "' not found.");
    }

    // Look for the variable name within the found group
    if (var_group_def->variables.count(expr.name)) {
        // The value of a variable is an expression itself, so we evaluate it.
        // This allows variables to be defined by other variables or complex expressions.
        result = evaluate(var_group_def->variables.at(expr.name).get(), this->current_context);
        return;
    }

    throw std::runtime_error("Variable '" + expr.name + "' not found in group '" + expr.group + "'.");
}

void ExpressionEvaluator::visit(ReferenceExpr& expr) {
    bool is_self_reference = expr.selector.lexeme.empty();

    if (is_self_reference) {
        if (!this->current_context) {
            throw std::runtime_error("Cannot self-reference property '" + expr.property.lexeme + "' outside of an element's style block.");
        }

        // For a self-reference, first check for circular dependencies.
        std::string self_ref_name = "self." + expr.property.lexeme;
        if (resolution_stack.count(self_ref_name)) {
            throw std::runtime_error("Circular property reference detected: " + self_ref_name);
        }

        // Then, compute the styles of the current element to see if the property exists.
        CHTLGenerator temp_generator(this->templates);
        std::map<std::string, AttributeNode> final_styles = temp_generator.computeFinalStyles(*this->current_context);

        if (final_styles.count(expr.property.lexeme)) {
            // It's a real property reference. Evaluate it.
            resolution_stack.insert(self_ref_name);
            result = evaluate(final_styles.at(expr.property.lexeme).value_expr.get(), this->current_context);
            resolution_stack.erase(self_ref_name);
            return;
        } else {
            // The property does not exist, so treat it as an unquoted string literal (e.g., "blue", "solid").
            result = {true, 0.0, "", expr.property.lexeme};
            return;
        }

    } else { // It's a reference to another element
        ElementNode* target_element = findElement(doc_root, expr.selector.lexeme);
        if (!target_element) {
            throw std::runtime_error("Reference selector '" + expr.selector.lexeme + "' not found.");
        }

        std::string full_ref_name = expr.selector.lexeme + "." + expr.property.lexeme;
        if (resolution_stack.count(full_ref_name)) {
            throw std::runtime_error("Circular property reference detected: " + full_ref_name);
        }

        CHTLGenerator temp_generator(this->templates);
        std::map<std::string, AttributeNode> final_styles = temp_generator.computeFinalStyles(*target_element);

        if (final_styles.count(expr.property.lexeme)) {
            resolution_stack.insert(full_ref_name);
            result = evaluate(final_styles.at(expr.property.lexeme).value_expr.get(), target_element);
            resolution_stack.erase(full_ref_name);
            return;
        }
        throw std::runtime_error("Reference property '" + expr.property.lexeme + "' not found in selector '" + expr.selector.lexeme + "'.");
    }
}

void ExpressionEvaluator::visit(ComparisonExpr& expr) {
    PropertyValue left = evaluate(expr.left.get(), this->current_context);
    PropertyValue right = evaluate(expr.right.get(), this->current_context);
    bool comparison_result = false;

    if (left.is_string || right.is_string) {
        if (expr.op.type == TokenType::EQUAL_EQUAL) {
            comparison_result = left.toString() == right.toString();
        } else if (expr.op.type == TokenType::BANG_EQUAL) {
            comparison_result = left.toString() != right.toString();
        } else {
            throw std::runtime_error("Can only perform equality checks (==, !=) on strings.");
        }
    } else {
        switch (expr.op.type) {
            case TokenType::GREATER:       comparison_result = left.numeric_value > right.numeric_value; break;
            case TokenType::GREATER_EQUAL: comparison_result = left.numeric_value >= right.numeric_value; break;
            case TokenType::LESS:          comparison_result = left.numeric_value < right.numeric_value; break;
            case TokenType::LESS_EQUAL:    comparison_result = left.numeric_value <= right.numeric_value; break;
            case TokenType::EQUAL_EQUAL:   comparison_result = left.numeric_value == right.numeric_value && left.unit == right.unit; break;
            case TokenType::BANG_EQUAL:    comparison_result = left.numeric_value != right.numeric_value || left.unit != right.unit; break;
            default: break;
        }
    }
    result = {false, comparison_result ? 1.0 : 0.0, "", ""};
}

void ExpressionEvaluator::visit(LogicalExpr& expr) {
    PropertyValue left = evaluate(expr.left.get(), this->current_context);
    if (expr.op.type == TokenType::PIPE_PIPE) {
        if (isTruthy(left)) { result = left; return; }
    } else { // AMPERSAND_AMPERSAND
        if (!isTruthy(left)) { result = left; return; }
    }
    result = evaluate(expr.right.get(), this->current_context);
}

void ExpressionEvaluator::visit(ConditionalExpr& expr) {
    PropertyValue condition = evaluate(expr.condition.get(), this->current_context);
    if (isTruthy(condition)) {
        result = evaluate(expr.then_branch.get(), this->current_context);
    } else {
        if (expr.else_branch) {
            result = evaluate(expr.else_branch.get(), this->current_context);
        } else {
            result = {false, 0.0, "", ""};
        }
    }
}

// Note: This findElement implementation is simplified. It does not currently
// support descendant selectors or indexed access, as the CHTL expression
// parser does not yet produce ASTs for them. It only supports a single
// simple selector (e.g., #id, .class, tag).
ElementNode* ExpressionEvaluator::findElement(BaseNode* root_node, const std::string& selector) {
    if (selector.empty() || !root_node) {
        return nullptr;
    }

    std::vector<ElementNode*> results;
    char selector_type = selector[0];

    // The parser currently only supports simple selectors in expressions.
    // Auto-inference logic for bare identifiers is included as specified.
    if (selector_type != '.' && selector_type != '#') { // Auto-inference: tag -> id -> class
        findElementsRecursive(root_node, selector, results);
        if (results.empty()) {
            findElementsRecursive(root_node, "#" + selector, results);
        }
        if (results.empty()) {
            findElementsRecursive(root_node, "." + selector, results);
        }
    } else { // Direct class or ID search
        findElementsRecursive(root_node, selector, results);
    }

    return results.empty() ? nullptr : results[0];
}

} // namespace CHTL
