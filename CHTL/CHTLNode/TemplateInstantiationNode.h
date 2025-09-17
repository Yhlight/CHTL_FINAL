#pragma once

#include "Node.h"
#include "../CHTLLexer/Token.h"
#include <string>

namespace CHTL {

class TemplateInstantiationNode : public Statement {
public:
    TemplateInstantiationNode(Token token, Token type, Token name)
        : m_token(std::move(token)),
          m_templateType(std::move(type)),
          m_name(std::move(name)) {}

    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + m_templateType.literal + " " + m_name.literal + ";\n";
    }

    Token m_token; // The '@' token that starts the instantiation
    Token m_templateType; // e.g., @Element, @Style
    Token m_name; // The name of the template to instantiate
};

} // namespace CHTL
