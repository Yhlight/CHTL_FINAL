#pragma once

#include "../CHTLLexer/Token.hpp"
#include <memory>
#include <vector>

namespace CHTL {
namespace CSSExt {

// Forward declare visitor and node types
class ExprVisitor;
class Binary;
class Grouping;
class Literal;
class Unary;

class Expression {
public:
    virtual ~Expression() = default;
    virtual void accept(ExprVisitor& visitor) = 0;
};

using ExprPtr = std::shared_ptr<Expression>;

class Binary : public Expression {
public:
    Binary(ExprPtr left, Token op, ExprPtr right)
        : left(std::move(left)), op(op), right(std::move(right)) {}

    void accept(ExprVisitor& visitor) override;

    const ExprPtr left;
    const Token op;
    const ExprPtr right;
};

class Grouping : public Expression {
public:
    Grouping(ExprPtr expression) : expression(std::move(expression)) {}

    void accept(ExprVisitor& visitor) override;

    const ExprPtr expression;
};

class Literal : public Expression {
public:
    Literal(Token value) : value(value) {}

    void accept(ExprVisitor& visitor) override;

    const Token value;
};

class Unary : public Expression {
public:
    Unary(Token op, ExprPtr right) : op(op), right(std::move(right)) {}

    void accept(ExprVisitor& visitor) override;

    const Token op;
    const ExprPtr right;
};


class ExprVisitor {
public:
    virtual ~ExprVisitor() = default;
    virtual void visit(Binary& expr) = 0;
    virtual void visit(Grouping& expr) = 0;
    virtual void visit(Literal& expr) = 0;
    virtual void visit(Unary& expr) = 0;
};

} // namespace CSSExt
} // namespace CHTL
