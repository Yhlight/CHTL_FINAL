#ifndef CHTL_EXPRESSION_NODE_H
#define CHTL_EXPRESSION_NODE_H

#include <memory>
#include <variant>
#include "../CHTLLexer/Token.h"
#include "PropertyReferenceNode.h"

namespace CHTL {

// Forward declarations
struct BinaryExpression;
struct ConditionalExpression;
struct LiteralExpression;
struct ReferenceExpression;

// Visitor pattern for expressions
class ExpressionVisitor {
public:
    virtual ~ExpressionVisitor() = default;
    virtual void visit(const BinaryExpression& expr) = 0;
    virtual void visit(const ConditionalExpression& expr) = 0;
    virtual void visit(const LiteralExpression& expr) = 0;
    virtual void visit(const ReferenceExpression& expr) = 0;
};

// Base class for all expression nodes
struct ExpressionNode {
    virtual ~ExpressionNode() = default;
    virtual void accept(ExpressionVisitor& visitor) const = 0;
    virtual std::unique_ptr<ExpressionNode> clone() const = 0;
};

// Expression for literal values (e.g., "10px", "red", 5)
struct LiteralExpression : public ExpressionNode {
    Token value;

    LiteralExpression(Token val) : value(std::move(val)) {}

    void accept(ExpressionVisitor& visitor) const override {
        visitor.visit(*this);
    }
    std::unique_ptr<ExpressionNode> clone() const override {
        return std::make_unique<LiteralExpression>(value);
    }
};

// Expression for property references (e.g., #box.width)
struct ReferenceExpression : public ExpressionNode {
    PropertyReferenceNode reference;

    ReferenceExpression(PropertyReferenceNode ref) : reference(std::move(ref)) {}

    void accept(ExpressionVisitor& visitor) const override {
        visitor.visit(*this);
    }
    std::unique_ptr<ExpressionNode> clone() const override {
        return std::make_unique<ReferenceExpression>(reference);
    }
};

// Expression for binary operations (e.g., width + 10px)
struct BinaryExpression : public ExpressionNode {
    std::unique_ptr<ExpressionNode> left;
    Token op;
    std::unique_ptr<ExpressionNode> right;

    BinaryExpression(std::unique_ptr<ExpressionNode> l, Token o, std::unique_ptr<ExpressionNode> r)
        : left(std::move(l)), op(std::move(o)), right(std::move(r)) {}

    void accept(ExpressionVisitor& visitor) const override {
        visitor.visit(*this);
    }
    std::unique_ptr<ExpressionNode> clone() const override {
        return std::make_unique<BinaryExpression>(left->clone(), op, right->clone());
    }
};

// Expression for conditional (ternary) operations
struct ConditionalExpression : public ExpressionNode {
    std::unique_ptr<ExpressionNode> condition;
    std::unique_ptr<ExpressionNode> thenBranch;
    std::unique_ptr<ExpressionNode> elseBranch; // Can be nullptr

    ConditionalExpression(std::unique_ptr<ExpressionNode> cond, std::unique_ptr<ExpressionNode> thenB, std::unique_ptr<ExpressionNode> elseB)
        : condition(std::move(cond)), thenBranch(std::move(thenB)), elseBranch(std::move(elseB)) {}

    void accept(ExpressionVisitor& visitor) const override {
        visitor.visit(*this);
    }
    std::unique_ptr<ExpressionNode> clone() const override {
        return std::make_unique<ConditionalExpression>(
            condition->clone(),
            thenBranch->clone(),
            elseBranch ? elseBranch->clone() : nullptr
        );
    }
};

} // namespace CHTL

#endif // CHTL_EXPRESSION_NODE_H
