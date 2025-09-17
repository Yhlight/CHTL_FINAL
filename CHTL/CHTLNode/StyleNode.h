#pragma once

#include "Node.h"
#include <vector>
#include <memory>

namespace CHTL {

class StyleNode : public Statement {
public:
    StyleNode(Token token) : m_token(std::move(token)) {}

    std::string toString(int indent = 0) const override {
        std::string out = std::string(indent, ' ') + "style {\n";
        for (const auto& prop : children) {
            out += prop->toString(indent + 2);
        }
        out += std::string(indent, ' ') + "}\n";
        return out;
    }

    Token m_token; // The 'style' token
    std::vector<std::unique_ptr<Statement>> children;
};

} // namespace CHTL
