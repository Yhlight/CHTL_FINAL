#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class AttributeNode : public Node {
public:
    Token key;
    std::unique_ptr<Expression> value;
    std::string tokenLiteral() const override { return key.literal; }
    std::string toString() const override;
};

class TextNode : public Statement {
public:
    Token token;
    std::string value;
    std::string tokenLiteral() const override { return token.literal; }
    std::string toString() const override;
};

class ConditionalExpression : public Expression {
public:
    Token token; // The '?' token
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Expression> consequence;
    std::unique_ptr<Expression> alternative;

    std::string tokenLiteral() const override { return token.literal; }
    std::string toString() const override;
};

class CommentNode : public Statement {
public:
    Token token;
    std::string value;
    std::string tokenLiteral() const override { return token.literal; }
    std::string toString() const override;
};

class ElementNode : public Statement {
public:
    Token token;
    std::vector<std::unique_ptr<AttributeNode>> attributes;
    std::vector<std::unique_ptr<Statement>> children;
    std::string tokenLiteral() const override { return token.literal; }
    std::string toString() const override;
};

class Identifier : public Expression {
public:
    Token token;
    std::string value;
    std::string tokenLiteral() const override { return token.literal; }
    std::string toString() const override { return value; }
};

class StringLiteral : public Expression {
public:
    Token token;
    std::string value;
    std::string tokenLiteral() const override { return token.literal; }
    std::string toString() const override { return token.literal; }
};

class NumberLiteral : public Expression {
public:
    Token token; // The token.type is NUMBER_LITERAL
    std::string value; // Store as string to preserve units like 'px', '%' etc.

    std::string tokenLiteral() const override { return token.literal; }
    std::string toString() const override { return value; }
};

class InfixExpression : public Expression {
public:
    Token token; // The operator token, e.g. +
    std::unique_ptr<Expression> left;
    std::string op;
    std::unique_ptr<Expression> right;

    std::string tokenLiteral() const override { return token.literal; }
    std::string toString() const override;
};

// Represents a reference to another property, e.g., #box.width
class PropertyReferenceExpression : public Expression {
public:
    Token token; // The '.' token
    std::unique_ptr<Expression> target; // The element being referenced, e.g., #box
    Identifier property; // The property being accessed, e.g., width

    std::string tokenLiteral() const override { return "."; }
    std::string toString() const override;
};

// Represents a variable substitution, e.g., ThemeColor(tableColor)
class VariableSubstitutionNode : public Expression {
public:
    Token token; // The '(' token
    std::unique_ptr<Expression> var_group; // The variable group name, e.g., ThemeColor
    Identifier variable; // The variable being accessed, e.g., tableColor

    std::string tokenLiteral() const override { return "("; }
    std::string toString() const override;
};

// Represents an indexed access, e.g., div[0]
class IndexedExpressionNode : public Expression {
public:
    Token token; // The '[' token
    std::unique_ptr<Expression> target; // The expression being indexed
    std::unique_ptr<Expression> index;

    std::string tokenLiteral() const override { return "["; }
    std::string toString() const override;
};

// Represents a local script { ... } block
class LocalScriptNode : public Statement {
public:
    Token token; // The 'script' token
    std::string raw_content; // The un-compiled CHTL JS source code

    std::string tokenLiteral() const override { return token.literal; }
    std::string toString() const override { return "script { ... }"; }
};

}
