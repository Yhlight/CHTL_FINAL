#ifndef CHTL_EXPRNODE_H
#define CHTL_EXPRNODE_H

#include <string>
#include <memory>

namespace CHTL
{
    enum class ExprType
    {
        BINARY_OP,
        NUMBER_LITERAL,
        STRING_LITERAL,
        ATTRIBUTE_REFERENCE
    };

    // Base class for all expression nodes
    class ExprNode
    {
    public:
        virtual ~ExprNode() = default;
        virtual ExprType getType() const = 0;
        virtual std::string toString() const = 0;
    };

    // Node for a numeric literal, e.g., 100 or 3.14px
    class NumberLiteralNode : public ExprNode
    {
    public:
        double value;
        std::string unit; // e.g., "px", "em", ""

        NumberLiteralNode(double val, std::string u) : value(val), unit(std::move(u)) {}

        ExprType getType() const override { return ExprType::NUMBER_LITERAL; }
        std::string toString() const override;
    };

    // Node for a binary operation, e.g., + - * /
    class BinaryOpNode : public ExprNode
    {
    public:
        char op; // e.g., '+', '-', '*', '/'
        std::unique_ptr<ExprNode> left;
        std::unique_ptr<ExprNode> right;

        BinaryOpNode(char o, std::unique_ptr<ExprNode> l, std::unique_ptr<ExprNode> r)
            : op(o), left(std::move(l)), right(std::move(r)) {}

        ExprType getType() const override { return ExprType::BINARY_OP; }
        std::string toString() const override;
    };

    // Node for a string/identifier literal, e.g., "hello" or "red"
    class StringLiteralNode : public ExprNode
    {
    public:
        std::string value;

        StringLiteralNode(std::string val) : value(std::move(val)) {}

        ExprType getType() const override { return ExprType::STRING_LITERAL; }
        std::string toString() const override;
    };

    // Node for a reference to another element's attribute, e.g., box.width
    class AttributeReferenceNode : public ExprNode
    {
    public:
        std::string selector;
        std::string propertyName;

        AttributeReferenceNode(std::string sel, std::string prop)
            : selector(std::move(sel)), propertyName(std::move(prop)) {}

        ExprType getType() const override { return ExprType::ATTRIBUTE_REFERENCE; }
        std::string toString() const override;
    };
}

#endif // CHTL_EXPRNODE_H
