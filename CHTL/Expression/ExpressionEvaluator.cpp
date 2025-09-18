#include "ExpressionEvaluator.h"
#include <iostream>
#include <cmath>

namespace CHTL {

ExpressionEvaluator::ExpressionEvaluator(const std::map<std::string, TemplateDefinitionNode>& templates)
    : templates(templates) {}

EvaluatedValue ExpressionEvaluator::evaluate(Expr* expr) {
    if (expr) {
        expr->accept(*this);
    }
    return result;
}

void ExpressionEvaluator::visit(LiteralExpr& expr) {
    // For a literal, the result is just its value and unit.
    // If the unit field contains a string that was parsed from a STRING or IDENTIFIER token,
    // the value is 0 and the unit is the string itself. We handle this by checking if the unit is empty.
    if (expr.unit.empty() || expr.value != 0) {
         result = {expr.value, expr.unit};
    } else {
        // It's a string-like literal, e.g., "sans-serif"
        result = {0, expr.unit};
    }
}

void ExpressionEvaluator::visit(BinaryExpr& expr) {
    // Evaluate the left and right sides of the expression first.
    EvaluatedValue left = evaluate(expr.left.get());
    EvaluatedValue right = evaluate(expr.right.get());

    // Check unit compatibility rule from CHTL.md
    if (!left.unit.empty() && !right.unit.empty() && left.unit != right.unit) {
        throw std::runtime_error("Mismatched units in expression: '" + left.unit + "' and '" + right.unit + "'.");
    }

    // The resulting unit will be the one that is not empty.
    std::string result_unit = !left.unit.empty() ? left.unit : right.unit;
    double result_value = 0.0;

    switch (expr.op.type) {
        case TokenType::PLUS:
            result_value = left.value + right.value;
            break;
        case TokenType::MINUS:
            result_value = left.value - right.value;
            break;
        case TokenType::STAR:
            result_value = left.value * right.value;
            break;
        case TokenType::SLASH:
            if (right.value == 0) throw std::runtime_error("Division by zero.");
            result_value = left.value / right.value;
            break;
        case TokenType::PERCENT:
            if (static_cast<int>(right.value) == 0) throw std::runtime_error("Modulo by zero.");
            result_value = fmod(left.value, right.value);
            break;
        case TokenType::STAR_STAR:
            result_value = pow(left.value, right.value);
            break;
        default:
            throw std::runtime_error("Unknown binary operator.");
    }

    result = {result_value, result_unit};
}

void ExpressionEvaluator::visit(VarExpr& expr) {
    std::string full_var_name = expr.group + "." + expr.name;

    // Circular dependency check
    if (resolution_stack.count(full_var_name)) {
        throw std::runtime_error("Circular variable reference detected for: " + full_var_name);
    }

    // Find the variable definition
    if (templates.count(expr.group)) {
        const auto& template_def = templates.at(expr.group);
        if (template_def.type == TemplateType::VAR && template_def.variables.count(expr.name)) {
            // Add to stack before recursive call
            resolution_stack.insert(full_var_name);
            // Recursively evaluate the variable's value
            result = evaluate(template_def.variables.at(expr.name).get());
            // Remove from stack after call returns
            resolution_stack.erase(full_var_name);
            return;
        }
    }

    throw std::runtime_error("Variable not found: " + full_var_name);
}

} // namespace CHTL
