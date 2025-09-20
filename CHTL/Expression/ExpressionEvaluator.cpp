#include "ExpressionEvaluator.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleNode.h"
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
        result = {ValueType::NUMERIC, expr.numeric_value, expr.unit, ""};
    } else { // STRING
        result = {ValueType::STRING, 0, "", expr.string_value};
    }
}

void ExpressionEvaluator::visit(BinaryExpr& expr) {
    EvaluatedValue left = evaluate(expr.left.get(), this->current_context);
    EvaluatedValue right = evaluate(expr.right.get(), this->current_context);

    if (left.type != ValueType::NUMERIC || right.type != ValueType::NUMERIC) {
        throw std::runtime_error("Arithmetic operations can only be performed on numeric values.");
    }

    double result_value = 0.0;
    std::string result_unit = "";

    switch (expr.op.type) {
        case TokenType::PLUS:
        case TokenType::MINUS:
            if (left.unit != right.unit) {
                // Allow adding/subtracting a unitless number from a number with a unit
                if (left.unit.empty()) {
                    result_unit = right.unit;
                } else if (right.unit.empty()) {
                    result_unit = left.unit;
                } else {
                    throw std::runtime_error("Mismatched units for + or -: '" + left.unit + "' and '" + right.unit + "'.");
                }
            } else {
                result_unit = left.unit;
            }
            result_value = (expr.op.type == TokenType::PLUS) ? (left.numeric_value + right.numeric_value) : (left.numeric_value - right.numeric_value);
            break;

        case TokenType::STAR:
            if (!left.unit.empty() && !right.unit.empty()) {
                throw std::runtime_error("Cannot multiply two values with units.");
            }
            result_unit = !left.unit.empty() ? left.unit : right.unit;
            result_value = left.numeric_value * right.numeric_value;
            break;

        case TokenType::SLASH:
            if (right.numeric_value == 0) throw std::runtime_error("Division by zero.");
            if (!right.unit.empty()) {
                throw std::runtime_error("Cannot divide by a value with a unit.");
            }
            result_unit = left.unit;
            result_value = left.numeric_value / right.numeric_value;
            break;

        case TokenType::PERCENT:
            if (static_cast<int>(right.numeric_value) == 0) throw std::runtime_error("Modulo by zero.");
             if (!right.unit.empty()) {
                throw std::runtime_error("Cannot modulo by a value with a unit.");
            }
            result_unit = left.unit;
            result_value = fmod(left.numeric_value, right.numeric_value);
            break;

        case TokenType::STAR_STAR:
            if (!right.unit.empty()) {
                 throw std::runtime_error("Cannot use a value with a unit as an exponent.");
            }
            result_unit = left.unit;
            result_value = pow(left.numeric_value, right.numeric_value);
            break;

        default:
            throw std::runtime_error("Unknown binary operator.");
    }

    result = {ValueType::NUMERIC, result_value, result_unit, ""};
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

    // Search for the property on the target element
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

    // If an implicit self-reference is not found as a property, treat it as a literal string.
    if (expr.selector.lexeme.empty()) {
        result = {ValueType::STRING, 0, "", expr.property.lexeme};
        return;
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
        if (left.unit != right.unit && expr.op.type != TokenType::EQUAL_EQUAL && expr.op.type != TokenType::BANG_EQUAL) {
            throw std::runtime_error("Cannot compare numeric values with different units: '" + left.unit + "' and '" + right.unit + "'.");
        }
        switch (expr.op.type) {
            case TokenType::GREATER:       comparison_result = left.numeric_value > right.numeric_value; break;
            case TokenType::GREATER_EQUAL: comparison_result = left.numeric_value >= right.numeric_value; break;
            case TokenType::LESS:          comparison_result = left.numeric_value < right.numeric_value; break;
            case TokenType::LESS_EQUAL:    comparison_result = left.numeric_value <= right.numeric_value; break;
            case TokenType::EQUAL_EQUAL:   comparison_result = (left.unit == right.unit) && (left.numeric_value == right.numeric_value); break;
            case TokenType::BANG_EQUAL:    comparison_result = (left.unit != right.unit) || (left.numeric_value != right.numeric_value); break;
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
        if (expr.else_branch) {
            result = evaluate(expr.else_branch.get(), this->current_context);
        } else {
            // No else branch, so this conditional produces no value.
            result = {ValueType::EMPTY, 0, "", ""};
        }
    }
}

ElementNode* ExpressionEvaluator::findElement(BaseNode* context, const std::string& selector) {
    if (!context) return nullptr;
    ElementNode* element = dynamic_cast<ElementNode*>(context);
    if (!element) return nullptr;

    char selector_type = selector.empty() ? ' ' : selector[0];
    if (selector_type == '#') {
        std::string id = selector.substr(1);
        for (const auto& attr : element->attributes) {
            if (attr.key == "id" && attr.value == id) return element;
        }
    } else if (selector_type == '.') {
        std::string className = selector.substr(1);
        for (const auto& attr : element->attributes) {
            if (attr.key == "class" && attr.value.find(className) != std::string::npos) return element;
        }
    } else {
        if (element->tagName == selector) return element;
    }

    for (const auto& child : element->children) {
        if (ElementNode* found = findElement(child.get(), selector)) return found;
    }

    return nullptr;
}

} // namespace CHTL
