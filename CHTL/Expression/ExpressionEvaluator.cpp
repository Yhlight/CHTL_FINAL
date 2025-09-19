#include "ExpressionEvaluator.h"
#include "Expr.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleNode.h"
#include <iostream>
#include <cmath>
#include <map>
#include <sstream>

namespace CHTL {

// Helper to format doubles cleanly for CSS output
std::string format_double(double val) {
    std::ostringstream oss;
    oss << val;
    return oss.str();
}

ExpressionEvaluator::ExpressionEvaluator(const std::map<std::string, std::map<std::string, TemplateDefinitionNode>>& templates, BaseNode* doc_root)
    : templates(templates), doc_root(doc_root) {}

std::string ExpressionEvaluator::evaluate(Expr* expr, ElementNode* context) {
    this->current_context = context;
    if (expr) {
        return expr->accept(*this);
    }
    return "";
}

std::string ExpressionEvaluator::visit(LiteralExpr& expr) {
    if (expr.unit.empty() || expr.value != 0) {
         return format_double(expr.value) + expr.unit;
    }
    // For cases like "red", "solid", "auto" where value is 0 but unit is the keyword
    return expr.unit;
}

std::string ExpressionEvaluator::visit(BinaryExpr& expr) {
    std::string left = evaluate(expr.left.get(), this->current_context);
    std::string right = evaluate(expr.right.get(), this->current_context);

    return left + " " + expr.op.lexeme + " " + right;
}

std::string ExpressionEvaluator::visit(GroupingExpr& expr) {
    return "(" + evaluate(expr.expression.get(), this->current_context) + ")";
}


std::string ExpressionEvaluator::visit(VarExpr& expr) {
    // This is a simplified implementation
    if (templates.count(expr.group) && templates.at(expr.group).count(expr.name)) {
        const auto& var_def = templates.at(expr.group).at(expr.name);
        if (var_def.variables.count(expr.name)) {
            return evaluate(var_def.variables.at(expr.name).get(), this->current_context);
        }
    }
    throw std::runtime_error("Variable not found: " + expr.group + "." + expr.name);
}

std::string ExpressionEvaluator::visit(ReferenceExpr& expr) {
    // This needs to be implemented to return a string, potentially a CSS variable
    // For now, returning an empty string as a placeholder.
    return "";
}

std::string ExpressionEvaluator::visit(ComparisonExpr& expr) {
    // Not directly applicable for CSS generation, but might be used in conditionals
    return "";
}

std::string ExpressionEvaluator::visit(LogicalExpr& expr) {
    // Not directly applicable for CSS generation
    return "";
}

std::string ExpressionEvaluator::visit(ConditionalExpr& expr) {
    // This will be complex, involving evaluating the condition and then returning
    // the string from the correct branch. Placeholder for now.
    return evaluate(expr.then_branch.get(), this->current_context);
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
