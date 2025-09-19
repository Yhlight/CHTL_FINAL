#include "ExpressionEvaluator.h"
#include "Expr.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleNode.h"
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
    // This is a simplified implementation
    if (templates.count(expr.group) && templates.at(expr.group).count(expr.name)) {
        const auto& var_def = templates.at(expr.group).at(expr.name);
        if (var_def.variables.count(expr.name)) {
            result = evaluate(var_def.variables.at(expr.name).get(), this->current_context);
            return;
        }
    }
    throw std::runtime_error("Variable not found: " + expr.group + "." + expr.name);
}

void ExpressionEvaluator::visit(ReferenceExpr& expr) {
    ElementNode* target_element = nullptr;
    bool is_self_reference = expr.selector.lexeme.empty();

    if (is_self_reference) {
        target_element = this->current_context;
        if (!target_element) {
            throw std::runtime_error("Cannot self-reference property '" + expr.property.lexeme + "' outside of an element's style block.");
        }
    } else {
        target_element = findElement(doc_root, expr.selector.lexeme);
        if (!target_element) {
            throw std::runtime_error("Reference selector '" + expr.selector.lexeme + "' not found.");
        }
    }

    std::string full_ref_name = (is_self_reference ? "self" : expr.selector.lexeme) + "." + expr.property.lexeme;
    if (resolution_stack.count(full_ref_name)) {
        throw std::runtime_error("Circular property reference detected: " + full_ref_name);
    }

    for (const auto& child : target_element->children) {
        if (StyleNode* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            for (const auto& prop : styleNode->direct_properties) {
                if (prop.key == expr.property.lexeme) {
                    resolution_stack.insert(full_ref_name);
                    result = evaluate(prop.value_expr.get(), target_element);
                    resolution_stack.erase(full_ref_name);
                    return;
                }
            }
        }
    }
    throw std::runtime_error("Reference property '" + expr.property.lexeme + "' not found in selector '" + expr.selector.lexeme + "'.");
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

ElementNode* ExpressionEvaluator::findElement(BaseNode* root_node, const std::string& full_selector) {
    std::vector<BaseNode*> search_roots = {root_node};
    std::vector<ElementNode*> final_results;

    std::stringstream ss(full_selector);
    std::string selector_part;

    // Process descendant selectors, which are space-separated.
    while (ss >> selector_part) {
        std::vector<ElementNode*> part_results;
        std::string name = selector_part;
        int index = -1;

        // Check for and parse indexed access, e.g., button[0]
        size_t bracket_pos = name.find('[');
        if (bracket_pos != std::string::npos) {
            size_t end_bracket_pos = name.find(']');
            if (end_bracket_pos != std::string::npos) {
                std::string index_str = name.substr(bracket_pos + 1, end_bracket_pos - bracket_pos - 1);
                try {
                    index = std::stoi(index_str);
                } catch (...) {
                    throw std::runtime_error("Invalid index in selector: " + selector_part);
                }
                name = name.substr(0, bracket_pos);
            }
        }

        // Search within the roots from the previous part
        for (BaseNode* root : search_roots) {
            char selector_type = name.empty() ? ' ' : name[0];
            if (selector_type != '.' && selector_type != '#') { // Auto-inference: tag -> id -> class
                // 1. Tag
                findElementsRecursive(root, name, part_results);
                // 2. ID
                if (part_results.empty()) {
                    findElementsRecursive(root, "#" + name, part_results);
                }
                // 3. Class
                if (part_results.empty()) {
                    findElementsRecursive(root, "." + name, part_results);
                }
            } else { // Direct class or ID search
                findElementsRecursive(root, name, part_results);
            }
        }

        // Filter by index if provided
        if (index != -1) {
            if (index >= 0 && index < part_results.size()) {
                final_results = {part_results[index]};
            } else {
                final_results.clear(); // Index out of bounds
            }
        } else {
            final_results = part_results;
        }

        if (final_results.empty()) return nullptr; // Early exit if any part of the selector fails

        search_roots.assign(final_results.begin(), final_results.end());
    }

    return final_results.empty() ? nullptr : final_results[0];
}

} // namespace CHTL
