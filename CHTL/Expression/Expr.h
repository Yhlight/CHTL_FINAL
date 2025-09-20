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
class ComparisonExpr;
class LogicalExpr;
class ConditionalExpr;
class ReactiveValueNode;
class ConcatExpr;
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
    virtual void visit(ComparisonExpr& expr) = 0;
    virtual void visit(LogicalExpr& expr) = 0;
    virtual void visit(ConditionalExpr& expr) = 0;
    virtual void visit(ReactiveValueNode& expr) = 0;
    virtual void visit(ConcatExpr& expr) = 0;
    // virtual void visit(GroupingExpr& expr) = 0; // For future use
};

// The base class for all expression nodes.
class Expr {
public:
    virtual ~Expr() = default;
    virtual void accept(ExprVisitor& visitor) = 0;
    virtual std::unique_ptr<Expr> clone() const = 0;
};

enum class LiteralType { NUMERIC, STRING };

// Represents a literal value, e.g., "100px" or "red".
class LiteralExpr : public Expr {
public:
    // Constructor for numeric literals
    LiteralExpr(double num_val, const std::string& unit_val)
        : type(LiteralType::NUMERIC), numeric_value(num_val), string_value(unit_val) {}

    // Constructor for string literals
    explicit LiteralExpr(const std::string& str_val)
        : type(LiteralType::STRING), numeric_value(0), string_value(str_val) {}

    void accept(ExprVisitor& visitor) override { visitor.visit(*this); }
    std::unique_ptr<Expr> clone() const override {
        if (type == LiteralType::NUMERIC) {
            return std::make_unique<LiteralExpr>(numeric_value, string_value);
        } else {
            return std::make_unique<LiteralExpr>(string_value);
        }
    }

    LiteralType type;
    double numeric_value;
    std::string string_value; // Used for the unit of a number, or the whole string value
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

// Represents a comparison, e.g., left > right
class ComparisonExpr : public Expr {
public:
    ComparisonExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}

    void accept(ExprVisitor& visitor) override { visitor.visit(*this); }
    std::unique_ptr<Expr> clone() const override {
        return std::make_unique<ComparisonExpr>(left->clone(), op, right->clone());
    }

    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;
};

// Represents a logical operation, e.g., left && right
class LogicalExpr : public Expr {
public:
    LogicalExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}

    void accept(ExprVisitor& visitor) override { visitor.visit(*this); }
    std::unique_ptr<Expr> clone() const override {
        return std::make_unique<LogicalExpr>(left->clone(), op, right->clone());
    }

    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;
};

// Represents a conditional (ternary) operation, e.g., cond ? then : else
class ConditionalExpr : public Expr {
public:
    ConditionalExpr(std::unique_ptr<Expr> condition, std::unique_ptr<Expr> then_branch, std::unique_ptr<Expr> else_branch)
        : condition(std::move(condition)), then_branch(std::move(then_branch)), else_branch(std::move(else_branch)) {}

    void accept(ExprVisitor& visitor) override { visitor.visit(*this); }
    std::unique_ptr<Expr> clone() const override {
        return std::make_unique<ConditionalExpr>(condition->clone(), then_branch->clone(), else_branch->clone());
    }

    std::unique_ptr<Expr> condition;
    std::unique_ptr<Expr> then_branch;
    std::unique_ptr<Expr> else_branch;
};

// Represents an implicit concatenation of two expressions, e.g., `box container`
class ConcatExpr : public Expr {
public:
    ConcatExpr(std::unique_ptr<Expr> left, std::unique_ptr<Expr> right)
        : left(std::move(left)), right(std::move(right)) {}

    void accept(ExprVisitor& visitor) override { visitor.visit(*this); }
    std::unique_ptr<Expr> clone() const override {
        return std::make_unique<ConcatExpr>(left->clone(), right->clone());
    }

    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
};

} // namespace CHTL

#endif // CHTL_EXPR_H
