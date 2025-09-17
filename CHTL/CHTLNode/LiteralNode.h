#pragma once

#include "Node.h"
#include "../CHTLLexer/Token.h"
#include <string>

namespace CHTL {

class LiteralNode : public Expression {
public:
    LiteralNode(Token token) : m_token(std::move(token)) {}
    // Return the raw literal. The generator will handle quoting if needed.
    std::string toString(int indent = 0) const override {
        (void)indent; // Unused parameter
        return m_token.literal;
    }
    Token m_token;
};

} // namespace CHTL
