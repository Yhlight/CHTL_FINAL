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
};

class Expr {
public:
    virtual ~Expr() = default;
    virtual void accept(ExprVisitor& visitor) = 0;
    virtual std::unique_ptr<Expr> clone() const = 0;
};

class LiteralExpr : public Expr {
public:
    LiteralExpr(double value, const std::string& unit) : value(value), unit(unit) {}
    void accept(ExprVisitor& visitor) override;
    std::unique_ptr<Expr> clone() const override;
    double value;
    std::string unit;
};

class BinaryExpr : public Expr {
public:
    BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}
    void accept(ExprVisitor& visitor) override;
    std::unique_ptr<Expr> clone() const override;
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;
};

class VarExpr : public Expr {
public:
    VarExpr(const std::string& group, const std::string& name) : group(group), name(name) {}
    void accept(ExprVisitor& visitor) override;
    std::unique_ptr<Expr> clone() const override;
    std::string group;
    std::string name;
};

class ReferenceExpr : public Expr {
public:
    ReferenceExpr(Token selector, Token property) : selector(selector), property(property) {}
    void accept(ExprVisitor& visitor) override;
    std::unique_ptr<Expr> clone() const override;
    Token selector;
    Token property;
};

class ComparisonExpr : public Expr {
public:
    ComparisonExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}
    void accept(ExprVisitor& visitor) override;
    std::unique_ptr<Expr> clone() const override;
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;
};

class LogicalExpr : public Expr {
public:
    LogicalExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}
    void accept(ExprVisitor& visitor) override;
    std::unique_ptr<Expr> clone() const override;
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;
};

class ConditionalExpr : public Expr {
public:
    ConditionalExpr(std::unique_ptr<Expr> condition, std::unique_ptr<Expr> then_branch, std::unique_ptr<Expr> else_branch)
        : condition(std::move(condition)), then_branch(std::move(then_branch)), else_branch(std::move(else_branch)) {}
    void accept(ExprVisitor& visitor) override;
    std::unique_ptr<Expr> clone() const override;
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Expr> then_branch;
    std::unique_ptr<Expr> else_branch;
};

} // namespace CHTL

#endif // CHTL_EXPR_H
