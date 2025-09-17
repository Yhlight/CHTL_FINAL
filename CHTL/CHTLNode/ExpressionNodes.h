#pragma once

#include "BaseNode.h"

namespace CHTL {

// Represents an identifier used as an expression, e.g., the value in `color: red;`
class Identifier : public Expression {
public:
    Token token; // The TOKEN_IDENTIFIER token

    explicit Identifier(Token t) : token(std::move(t)) {}

    std::string GetTokenLiteral() const override { return token.literal; }
    std::string ToString() const override { return token.literal; }
};

// Represents a string literal used as an expression
class StringLiteral : public Expression {
public:
    Token token; // The TOKEN_STRING token

    explicit StringLiteral(Token t) : token(std::move(t)) {}

    std::string GetTokenLiteral() const override { return token.literal; }
    std::string ToString() const override { return "\"" + token.literal + "\""; }
};

} // namespace CHTL
