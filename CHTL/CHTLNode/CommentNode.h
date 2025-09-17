#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

// Represents a # comment, which is preserved in the output
class CommentNode : public Statement {
public:
    Token token; // The TOKEN_COMMENT token

    explicit CommentNode(Token t) : token(std::move(t)) {}

    std::string GetTokenLiteral() const override {
        return token.literal;
    }

    std::string ToString() const override {
        return token.literal;
    }
};

} // namespace CHTL
