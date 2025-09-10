#pragma once

#include "BaseNode.h"
#include "../CHTLLexer/Token.h"

// Base class for all expression nodes
class ExprNode : public BaseNode {
public:
    virtual std::unique_ptr<ExprNode> clone() const = 0;
};

// Node for a literal value (number, string, or identifier with unit)
class LiteralNode : public ExprNode {
public:
    Token value;
    LiteralNode(const Token& value) : value(value) {}
    std::unique_ptr<ExprNode> clone() const override {
        return std::make_unique<LiteralNode>(value);
    }
};

// Node for a binary operation (e.g., 100 + 50)
class BinaryOpNode : public ExprNode {
public:
    std::unique_ptr<ExprNode> left;
    Token op;
    std::unique_ptr<ExprNode> right;

    BinaryOpNode(std::unique_ptr<ExprNode> left, Token op, std::unique_ptr<ExprNode> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}
    std::unique_ptr<ExprNode> clone() const override {
        return std::make_unique<BinaryOpNode>(left->clone(), op, right->clone());
    }
};

// Node for a unary operation (e.g., -10)
class UnaryOpNode : public ExprNode {
public:
    Token op;
    std::unique_ptr<ExprNode> right;

    UnaryOpNode(Token op, std::unique_ptr<ExprNode> right)
        : op(op), right(std::move(right)) {}
    std::unique_ptr<ExprNode> clone() const override {
        return std::make_unique<UnaryOpNode>(op, right->clone());
    }
};

// Node for a grouping (e.g., (100 + 50))
class GroupingNode : public ExprNode {
public:
    std::unique_ptr<ExprNode> expression;

    GroupingNode(std::unique_ptr<ExprNode> expression)
        : expression(std::move(expression)) {}
    std::unique_ptr<ExprNode> clone() const override {
        return std::make_unique<GroupingNode>(expression->clone());
    }
};

// Node for member access (e.g., #box.width)
class MemberAccessNode : public ExprNode {
public:
    std::unique_ptr<ExprNode> object; // The thing being accessed (e.g., #box)
    Token property; // The property name (e.g., width)

    MemberAccessNode(std::unique_ptr<ExprNode> object, Token property)
        : object(std::move(object)), property(property) {}
    std::unique_ptr<ExprNode> clone() const override {
        return std::make_unique<MemberAccessNode>(object->clone(), property);
    }
};

// Node for a ternary conditional expression (e.g., condition ? then : else)
class TernaryNode : public ExprNode {
public:
    std::unique_ptr<ExprNode> condition;
    std::unique_ptr<ExprNode> then_branch;
    std::unique_ptr<ExprNode> else_branch;

    TernaryNode(std::unique_ptr<ExprNode> condition, std::unique_ptr<ExprNode> then_branch, std::unique_ptr<ExprNode> else_branch)
        : condition(std::move(condition)), then_branch(std::move(then_branch)), else_branch(std::move(else_branch)) {}
    std::unique_ptr<ExprNode> clone() const override {
        return std::make_unique<TernaryNode>(condition->clone(), then_branch->clone(), else_branch->clone());
    }
};

// Node for a variable access (e.g., MyVars(myColor))
class VarAccessNode : public ExprNode {
public:
    Token group_name;
    Token var_name;

    VarAccessNode(Token group, Token var) : group_name(group), var_name(var) {}
    std::unique_ptr<ExprNode> clone() const override {
        return std::make_unique<VarAccessNode>(group_name, var_name);
    }
};
