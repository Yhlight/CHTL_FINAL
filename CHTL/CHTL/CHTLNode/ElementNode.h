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

}
