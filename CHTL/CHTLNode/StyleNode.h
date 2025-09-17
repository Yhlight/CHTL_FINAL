#pragma once

#include "BaseNode.h"
#include "AttributeNode.h"
#include <vector>
#include <memory>
#include <sstream>

namespace CHTL {

class StyleNode : public Statement {
public:
    Token token; // The 'style' keyword token
    std::vector<std::shared_ptr<AttributeNode>> properties;

    explicit StyleNode(Token t) : token(std::move(t)) {}

    std::string GetTokenLiteral() const override {
        return token.literal;
    }

    std::string ToString() const override {
        std::stringstream out;
        out << "style {\n";
        for (const auto& prop : properties) {
            out << "  " << prop->ToString() << "\n";
        }
        out << "}";
        return out.str();
    }
};

} // namespace CHTL
