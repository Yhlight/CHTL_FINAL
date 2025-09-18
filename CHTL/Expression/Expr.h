#ifndef CHTL_EXPR_H
#define CHTL_EXPR_H

#include "../CHTLLexer/Token.h"
#include <memory>
#include <string>
#include <vector>

namespace CHTL {

// Forward declare all expression types for the visitor
class BinaryExpr;
class LiteralExpr;
class VarExpr;
class ReferenceExpr;
class GroupingExpr; // For future use with parentheses

// The base visitor interface for the expression AST.
// It uses the "Visitor" design pattern to allow for different operations
// (like evaluation or printing) to be performed on the expression tree.
class ExprVisitor {
public:
    virtual ~ExprVisitor() = default;
    virtual void visit(BinaryExpr& expr) = 0;
    virtual void visit(LiteralExpr& expr) = 0;
    virtual void visit(VarExpr& expr) = 0;
    virtual void visit(ReferenceExpr& expr) = 0;
    // virtual void visit(GroupingExpr& expr) = 0; // For future use
};

// The base class for all expression nodes.
class Expr {
public:
    virtual ~Expr() = default;
    virtual void accept(ExprVisitor& visitor) = 0;
    virtual std::unique_ptr<Expr> clone() const = 0;
};

// Represents a literal value, e.g., "100" and "px".
// We store the value as a double to handle potential floating point math.
class LiteralExpr : public Expr {
public:
    LiteralExpr(double value, const std::string& unit)
        : value(value), unit(unit) {}

    void accept(ExprVisitor& visitor) override { visitor.visit(*this); }
    std::unique_ptr<Expr> clone() const override {
        return std::make_unique<LiteralExpr>(value, unit);
    }

    double value;
    std::string unit;
};

// Represents a binary operation, e.g., left + right.
class BinaryExpr : public Expr {
public:
    BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}

    void accept(ExprVisitor& visitor) override { visitor.visit(*this); }
    std::unique_ptr<Expr> clone() const override {
        return std::make_unique<BinaryExpr>(left->clone(), op, right->clone());
    }

    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;
};

// Represents a variable usage, e.g., ThemeColor(primary)
class VarExpr : public Expr {
public:
    VarExpr(const std::string& group, const std::string& name)
        : group(group), name(name) {}

    void accept(ExprVisitor& visitor) override { visitor.visit(*this); }
    std::unique_ptr<Expr> clone() const override {
        return std::make_unique<VarExpr>(group, name);
    }

    std::string group;
    std::string name;
};

// Represents a reference to another property, e.g., box.width
class ReferenceExpr : public Expr {
public:
    ReferenceExpr(Token selector, Token property)
        : selector(selector), property(property) {}

    void accept(ExprVisitor& visitor) override { visitor.visit(*this); }
    std::unique_ptr<Expr> clone() const override {
        return std::make_unique<ReferenceExpr>(selector, property);
    }

    Token selector;
    Token property;
};

} // namespace CHTL

#endif // CHTL_EXPR_H
