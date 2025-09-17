#pragma once

#include "BaseNode.h"
#include "ExpressionNodes.h"
#include <memory>
#include <utility>

namespace CHTL {

class AttributeNode : public Statement {
public:
    Token key; // The attribute's key (e.g., 'id', 'class')
    std::shared_ptr<Expression> value; // The attribute's value

    AttributeNode(Token k, std::shared_ptr<Expression> v)
        : key(std::move(k)), value(std::move(v)) {}

    std::string GetTokenLiteral() const override {
        return key.literal;
    }

    std::string ToString() const override {
        std::string out = GetTokenLiteral() + ": ";
        if (value) {
            out += value->ToString();
        }
        out += ";";
        return out;
    }
};

} // namespace CHTL
