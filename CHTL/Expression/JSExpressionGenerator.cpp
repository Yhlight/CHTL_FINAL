#include "JSExpressionGenerator.h"

namespace CHTL {

JSExpressionGenerator::JSExpressionGenerator() {}

std::string JSExpressionGenerator::generate(Expr* expr) {
    ss.str(""); // Clear the stream
    if (expr) {
        expr->accept(*this);
    }
    return ss.str();
}

void JSExpressionGenerator::visit(LiteralExpr& expr) {
    // For logical/numeric expressions, the unit is irrelevant.
    // We might need a different method for generating string concatenations.
    ss << expr.value;
}

void JSExpressionGenerator::visit(BinaryExpr& expr) {
    ss << "(";
    expr.left->accept(*this);
    ss << " " << expr.op.lexeme << " ";
    expr.right->accept(*this);
    ss << ")";
}

void JSExpressionGenerator::visit(ComparisonExpr& expr) {
    ss << "(";
    expr.left->accept(*this);
    ss << " " << expr.op.lexeme << " ";
    expr.right->accept(*this);
    ss << ")";
}

void JSExpressionGenerator::visit(LogicalExpr& expr) {
    ss << "(";
    expr.left->accept(*this);
    ss << " " << expr.op.lexeme << " ";
    expr.right->accept(*this);
    ss << ")";
}

void JSExpressionGenerator::visit(ConditionalExpr& expr) {
    ss << "(";
    expr.condition->accept(*this);
    ss << " ? ";
    expr.then_branch->accept(*this);
    ss << " : ";
    expr.else_branch->accept(*this);
    ss << ")";
}

void JSExpressionGenerator::visit(ReferenceExpr& expr) {
    // This is for static, self-referential properties.
    // In a dynamic JS context, this would likely translate to a variable name.
    // This part of the implementation might need to be more context-aware.
    ss << expr.property.lexeme;
}

void JSExpressionGenerator::visit(DynamicReferenceExpr& expr) {
    // This is a simplification. A robust implementation would need to parse
    // the CHTL selector and convert it to a valid JS querySelector.
    // For now, we assume simple selectors.
    std::string selector = expr.selector;
    if (selector.rfind(".", 0) != 0 && selector.rfind("#", 0) != 0) {
        selector = "#" + selector; // Assume ID if no prefix
    }

    // This is also a simplification. offsetWidth, textContent, etc. would depend on the property.
    ss << "parseFloat(document.querySelector('" << selector << "')." << expr.property << ")";
}

void JSExpressionGenerator::visit(VarExpr& expr) {
    // This assumes the variable has been declared in the JS scope.
    ss << expr.name;
}

} // namespace CHTL
