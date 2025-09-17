#pragma once

#include "Node.h"
#include "../CHTLLexer/Token.h"
#include <string>
#include <memory>

namespace CHTL {

class AttributeNode : public Statement {
public:
    AttributeNode(Token token, std::string key)
        : m_token(std::move(token)), m_key(std::move(key)) {}

    std::string toString(int indent = 0) const override {
        std::string out = std::string(indent, ' ');
        out += m_key;
        out += ": ";
        if (m_value) {
            out += m_value->toString(0); // Don't indent value
        }
        out += ";\n";
        return out;
    }

    Token m_token; // The token for the attribute's key (e.g., 'id')
    std::string m_key;
    std::unique_ptr<Expression> m_value;
};

} // namespace CHTL
