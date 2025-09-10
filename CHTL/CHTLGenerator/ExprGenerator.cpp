#include "ExprGenerator.h"

std::string ExprGenerator::generate(ExprNode* expr) {
    expr->accept(*this);
    return output.str();
}

void ExprGenerator::visit(LiteralExprNode* node) {
    output << node->value.value;
}

void ExprGenerator::visit(BinaryExprNode* node) {
    parenthesize(node->op.value, node->left.get());
    output << " " << node->op.value << " ";
    parenthesize(node->op.value, node->right.get());
}

void ExprGenerator::visit(TernaryExprNode* node) {
    parenthesize("?:", node->condition.get());
    output << " ? ";
    parenthesize("?:", node->trueExpr.get());
    output << " : ";
    parenthesize("?:", node->falseExpr.get());
}

void ExprGenerator::parenthesize(const std::string& name, ExprNode* expr) {
    // This is a simplified version. A full implementation would check
    // operator precedence to decide if parentheses are needed.
    // For now, we'll just wrap sub-expressions in binary/ternary nodes.
    output << "(";
    expr->accept(*this);
    output << ")";
}
