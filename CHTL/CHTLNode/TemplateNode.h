#pragma once

#include "Node.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class TemplateNode : public Statement {
public:
    TemplateNode(Token token, Token type, Token name)
        : m_token(std::move(token)),
          m_templateType(std::move(type)),
          m_name(std::move(name)) {}

    std::string toString(int indent = 0) const override {
        std::string out = std::string(indent, ' ') + "[Template] " + m_templateType.literal + " " + m_name.literal + " {\n";
        for (const auto& child : children) {
            out += child->toString(indent + 2);
        }
        out += std::string(indent, ' ') + "}\n";
        return out;
    }

    Token m_token; // The [Template] token
    Token m_templateType; // e.g., @Element, @Style
    Token m_name;
    std::vector<std::unique_ptr<Statement>> children;
};

} // namespace CHTL
