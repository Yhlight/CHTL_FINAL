#pragma once

#include "Node.h"
#include "../CHTLLexer/Token.h"
#include <string>

namespace CHTL {

class TextNode : public Statement {
public:
    TextNode(Token token, std::string value)
        : m_token(std::move(token)), m_value(std::move(value)) {}

    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "text: \"" + m_value + "\"\n";
    }

    Token m_token; // The 'text' token
    std::string m_value;
};

} // namespace CHTL
