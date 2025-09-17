#pragma once

#include "BaseNode.h"
#include <vector>
#include <memory>
#include <sstream>

namespace CHTL {

class ElementNode : public Statement {
public:
    Token token; // The token for the element's tag, e.g., 'div'
    std::vector<std::shared_ptr<Statement>> children; // Statements inside the braces

    ElementNode(Token t) : token(std::move(t)) {}

    std::string GetTokenLiteral() const override {
        return token.literal;
    }

    std::string ToString() const override {
        std::stringstream out;
        out << GetTokenLiteral() << " {\n";
        for (const auto& child : children) {
            out << child->ToString() << "\n";
        }
        out << "}";
        return out.str();
    }
};

} // namespace CHTL
