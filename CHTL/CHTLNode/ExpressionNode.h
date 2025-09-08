#pragma once

#include "BaseNode.h"
#include "../CHTLLexer/Token.h"
#include <string>
#include <memory>
#include <utility>

// Base class for all expression nodes
class ExpressionNode : public BaseNode {
public:
    void accept(AstVisitor& visitor) override {
        // This might need a new visitor type, an ExpressionVisitor.
        // For now, we leave it empty as it's not part of the main AST traversal.
    }
};

// Represents a literal value (e.g., "red", 100, 16px)
class LiteralNode : public ExpressionNode {
public:
    explicit LiteralNode(Token val) : value(std::move(val)) {}

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<LiteralNode>(this->value);
    }

    Token value;
};

// Represents access to a property (e.g., width, .box.width)
class PropertyAccessNode : public ExpressionNode {
public:
    explicit PropertyAccessNode(Token prop) : property(std::move(prop)) {}

     std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<PropertyAccessNode>(this->property);
    }

    Token property;
};

// Represents a binary operation (e.g., width > 50)
class BinaryOpNode : public ExpressionNode {
public:
    BinaryOpNode(std::unique_ptr<ExpressionNode> l, Token op, std::unique_ptr<ExpressionNode> r)
        : left(std::move(l)), op(std::move(op)), right(std::move(r)) {}

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<BinaryOpNode>(
            std::unique_ptr<ExpressionNode>(static_cast<ExpressionNode*>(this->left->clone().release())),
            this->op,
            std::unique_ptr<ExpressionNode>(static_cast<ExpressionNode*>(this->right->clone().release()))
        );
    }

    std::unique_ptr<ExpressionNode> left;
    Token op;
    std::unique_ptr<ExpressionNode> right;
};

// Represents a ternary operation (e.g., condition ? true_expr : false_expr)
class TernaryOpNode : public ExpressionNode {
public:
    TernaryOpNode(std::unique_ptr<ExpressionNode> cond, std::unique_ptr<ExpressionNode> trueExpr, std::unique_ptr<ExpressionNode> falseExpr)
        : condition(std::move(cond)), trueExpression(std::move(trueExpr)), falseExpression(std::move(falseExpr)) {}

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<TernaryOpNode>(
            std::unique_ptr<ExpressionNode>(static_cast<ExpressionNode*>(this->condition->clone().release())),
            std::unique_ptr<ExpressionNode>(static_cast<ExpressionNode*>(this->trueExpression->clone().release())),
            std::unique_ptr<ExpressionNode>(static_cast<ExpressionNode*>(this->falseExpression->clone().release()))
        );
    }

    std::unique_ptr<ExpressionNode> condition;
    std::unique_ptr<ExpressionNode> trueExpression;
    std::unique_ptr<ExpressionNode> falseExpression;
};
