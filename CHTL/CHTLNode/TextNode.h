#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

class TextNode : public Statement {
public:
    Token token; // The TOKEN_STRING token containing the text content

    explicit TextNode(Token t) : token(std::move(t)) {}

    std::string GetTokenLiteral() const override {
        return token.literal;
    }

    std::string ToString() const override {
        return "text { \"" + token.literal + "\" }";
    }
};

} // namespace CHTL
