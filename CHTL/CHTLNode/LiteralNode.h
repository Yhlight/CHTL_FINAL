#ifndef CHTL_LITERALNODE_H
#define CHTL_LITERALNODE_H

#include "ExpressionNode.h"

class LiteralNode : public ExpressionNode {
public:
    // The token holds the value and type of the literal.
    // e.g., {NUMBER_LITERAL, "100px", ...} or {IDENTIFIER, "red", ...}

    // Constructor
    explicit LiteralNode(const Token& token) {
        this->token = token;
    }

    std::string debugString(int indent = 0) const override {
        return indentString(indent) + "LiteralNode: " + token.lexeme + "\n";
    }

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<LiteralNode>(*this);
    }
};

#endif //CHTL_LITERALNODE_H
