#ifndef CHTL_JS_EXPR_GENERATOR_H
#define CHTL_JS_EXPR_GENERATOR_H

#include "Expr.h"
#include <sstream>
#include <stdexcept>

namespace CHTL {

// A visitor to convert a CHTL expression tree into a JavaScript expression string.
class JSExprGenerator : public ExprVisitor {
public:
    std::string generate(Expr* expr) {
        if (expr) {
            expr->accept(*this);
        }
        return ss.str();
    }

    void visit(BinaryExpr& expr) override {
        ss << "(";
        expr.left->accept(*this);
        ss << " " << expr.op.lexeme << " ";
        expr.right->accept(*this);
        ss << ")";
    }

    void visit(LiteralExpr& expr) override {
        if (expr.is_string_literal) {
            ss << "\"" << expr.unit << "\"";
        } else {
            // JS doesn't have units, so we just use the value.
            // A more complex implementation might handle `px` etc.
            ss << expr.value;
        }
    }

    void visit(VarExpr& expr) override {
        // This assumes the variable group has been translated to a JS object
        ss << expr.group << "." << expr.name;
    }

    void visit(ReferenceExpr& expr) override {
        throw std::runtime_error("Static ReferenceExpr cannot be converted to JS.");
    }

    void visit(ComparisonExpr& expr) override {
        ss << "(";
        expr.left->accept(*this);
        ss << " " << expr.op.lexeme << " ";
        expr.right->accept(*this);
        ss << ")";
    }

    void visit(LogicalExpr& expr) override {
        ss << "(";
        expr.left->accept(*this);
        ss << " " << expr.op.lexeme << " ";
        expr.right->accept(*this);
        ss << ")";
    }

    void visit(ConditionalExpr& expr) override {
        ss << "(";
        expr.condition->accept(*this);
        ss << " ? ";
        expr.then_branch->accept(*this);
        ss << " : ";
        expr.else_branch->accept(*this);
        ss << ")";
    }

    void visit(DynamicReferenceExpr& expr) override {
        // This is a simplification. A full implementation would need a runtime
        // function to get computed styles. e.g., CHTL.getStyle('selector', 'property')
        ss << "CHTL.getStyle('" << expr.selector << "', '" << expr.property << "')";
    }

    void visit(ReactiveVarExpr& expr) override {
        ss << expr.name;
    }

private:
    std::stringstream ss;
};

} // namespace CHTL

#endif // CHTL_JS_EXPR_GENERATOR_H
