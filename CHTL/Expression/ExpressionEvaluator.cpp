#include "ExpressionEvaluator.h"
#include "Expr.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleNode.h"
#include <iostream>
#include <cmath>
#include <map>

namespace CHTL {

static bool isTruthy(const EvaluatedValue& val) {
    if (!val.unit.empty() && val.unit != "0") return true;
    return val.value != 0;
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
    if (expr.unit.empty() || expr.value != 0) {
         result = {expr.value, expr.unit};
    } else {
        result = {0, expr.unit};
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
    std::string full_ref_name = expr.selector.lexeme + "." + expr.property.lexeme;
    if (resolution_stack.count(full_ref_name)) {
        throw std::runtime_error("Circular property reference detected.");
    }
    ElementNode* target_element = findElement(doc_root, expr.selector.lexeme);
    if (!target_element) {
        throw std::runtime_error("Reference selector not found.");
    }

    for (const auto& child : target_element->children) {
        if (StyleNode* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            std::map<std::string, AttributeNode> final_props;
            for (const auto& app : styleNode->template_applications) {
                // ... logic to apply templates ...
            }
            for (const auto& prop : styleNode->direct_properties) {
                final_props[prop.key] = prop.clone();
            }
            if (final_props.count(expr.property.lexeme)) {
                resolution_stack.insert(full_ref_name);
                result = evaluate(final_props.at(expr.property.lexeme).value_expr.get(), target_element);
                resolution_stack.erase(full_ref_name);
                return;
            }
        }
    }
    throw std::runtime_error("Reference property not found.");
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
