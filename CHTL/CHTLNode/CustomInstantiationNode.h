#pragma once

#include "Node.h"
#include "../CHTLLexer/Token.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

// Represents a template instantiation that has a body for specialization, e.g.
// @Style MyTemplate { color: red; }
class CustomInstantiationNode : public Statement {
public:
    CustomInstantiationNode(Token type, Token name)
        : m_templateType(std::move(type)),
          m_name(std::move(name)) {}

    std::string toString(int indent = 0) const override {
        std::string out = std::string(indent, ' ') + m_templateType.literal + " " + m_name.literal + " {\n";
        for (const auto& child : children) {
            out += child->toString(indent + 2);
        }
        out += std::string(indent, ' ') + "}\n";
        return out;
    }

    Token m_templateType; // e.g., @Element, @Style
    Token m_name; // The name of the template to instantiate
    std::vector<std::shared_ptr<Statement>> children; // The specialization rules
};

} // namespace CHTL
