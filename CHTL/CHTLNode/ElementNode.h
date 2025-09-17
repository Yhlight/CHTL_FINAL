#pragma once

#include "Node.h"
#include "AttributeNode.h"
#include "StyleNode.h"
#include "../CHTLLexer/Token.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class ElementNode : public Statement {
public:
    ElementNode(Token token, std::string tagName)
        : m_token(std::move(token)), m_tagName(std::move(tagName)) {}

    std::string toString(int indent = 0) const override {
        std::string out = std::string(indent, ' ') + m_tagName + " {\n";

        for (const auto& attr : attributes) {
            out += attr->toString(indent + 2);
        }

        if (styleBlock) {
            out += styleBlock->toString(indent + 2);
        }

        for (const auto& child : children) {
            out += child->toString(indent + 2);
        }

        out += std::string(indent, ' ') + "}\n";
        return out;
    }

    Token m_token; // The token for the element's tag name (e.g., 'div')
    std::string m_tagName;
    std::vector<std::unique_ptr<AttributeNode>> attributes;
    std::unique_ptr<StyleNode> styleBlock;
    std::vector<std::unique_ptr<Statement>> children;
};

} // namespace CHTL
