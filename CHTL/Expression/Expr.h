#ifndef EXPR_H
#define EXPR_H

#include "../CHTLLexer/Token.h"
#include <memory>
#include <string>
#include <vector>

namespace CHTL {

struct BinaryExpr;
struct LiteralExpr;
struct VarExpr;
struct ReferenceExpr;
struct ComparisonExpr;
struct LogicalExpr;
struct ConditionalExpr;
struct GroupingExpr;

class ExprVisitorString {
public:
    virtual ~ExprVisitorString() = default;
    virtual std::string visit(BinaryExpr& expr) = 0;
    virtual std::string visit(LiteralExpr& expr) = 0;
    virtual std::string visit(VarExpr& expr) = 0;
    virtual std::string visit(ReferenceExpr& expr) = 0;
    virtual std::string visit(ComparisonExpr& expr) = 0;
    virtual std::string visit(LogicalExpr& expr) = 0;
    virtual std::string visit(ConditionalExpr& expr) = 0;
    virtual std::string visit(GroupingExpr& expr) = 0;
};

class Expr {
public:
    virtual ~Expr() = default;
    virtual std::string accept(ExprVisitorString& visitor) = 0;
    virtual std::unique_ptr<Expr> clone() const = 0;
};

struct BinaryExpr : Expr {
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;

    BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}

    std::string accept(ExprVisitorString& visitor) override { return visitor.visit(*this); }
    std::unique_ptr<Expr> clone() const override {
        return std::make_unique<BinaryExpr>(left->clone(), op, right->clone());
    }
};

struct GroupingExpr : Expr {
    std::unique_ptr<Expr> expression;

    GroupingExpr(std::unique_ptr<Expr> expression) : expression(std::move(expression)) {}

    std::string accept(ExprVisitorString& visitor) override { return visitor.visit(*this); }
    std::unique_ptr<Expr> clone() const override {
        return std::make_unique<GroupingExpr>(expression->clone());
    }
};

struct LiteralExpr : Expr {
    double value;
    std::string unit;

    LiteralExpr(double value, std::string unit) : value(value), unit(unit) {}

    std::string accept(ExprVisitorString& visitor) override { return visitor.visit(*this); }
    std::unique_ptr<Expr> clone() const override {
        return std::make_unique<LiteralExpr>(value, unit);
    }
};

struct VarExpr : Expr {
    std::string group;
    std::string name;

    VarExpr(std::string group, std::string name) : group(std::move(group)), name(std::move(name)) {}

    std::string accept(ExprVisitorString& visitor) override { return visitor.visit(*this); }
    std::unique_ptr<Expr> clone() const override {
        return std::make_unique<VarExpr>(group, name);
    }
};

struct ReferenceExpr : Expr {
    Token selector;
    Token property;

    ReferenceExpr(Token selector, Token property) : selector(selector), property(property) {}

    std::string accept(ExprVisitorString& visitor) override { return visitor.visit(*this); }
    std::unique_ptr<Expr> clone() const override {
        return std::make_unique<ReferenceExpr>(selector, property);
    }
};

struct ComparisonExpr : Expr {
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;

    ComparisonExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}

    std::string accept(ExprVisitorString& visitor) override { return visitor.visit(*this); }
    std::unique_ptr<Expr> clone() const override {
        return std::make_unique<ComparisonExpr>(left->clone(), op, right->clone());
    }
};

struct LogicalExpr : Expr {
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;

    LogicalExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}

    std::string accept(ExprVisitorString& visitor) override { return visitor.visit(*this); }
    std::unique_ptr<Expr> clone() const override {
        return std::make_unique<LogicalExpr>(left->clone(), op, right->clone());
    }
};

struct ConditionalExpr : Expr {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Expr> then_branch;
    std::unique_ptr<Expr> else_branch;

    ConditionalExpr(std::unique_ptr<Expr> condition, std::unique_ptr<Expr> then_branch, std::unique_ptr<Expr> else_branch)
        : condition(std::move(condition)), then_branch(std::move(then_branch)), else_branch(std::move(else_branch)) {}

    std::string accept(ExprVisitorString& visitor) override { return visitor.visit(*this); }
    std::unique_ptr<Expr> clone() const override {
        return std::make_unique<ConditionalExpr>(condition->clone(), then_branch->clone(), else_branch->clone());
    }
};

} // namespace CHTL

#endif // EXPR_H
