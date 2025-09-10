#ifndef CHTL_BINARYOPNODE_H
#define CHTL_BINARYOPNODE_H

#include "ExpressionNode.h"
#include <memory>
#include <sstream>

class BinaryOpNode : public ExpressionNode {
public:
    std::unique_ptr<ExpressionNode> left;
    Token op; // The operator token (e.g., PLUS, MINUS)
    std::unique_ptr<ExpressionNode> right;

    // Constructor
    BinaryOpNode(std::unique_ptr<ExpressionNode> left, const Token& op, std::unique_ptr<ExpressionNode> right)
        : left(std::move(left)), op(op), right(std::move(right)) {
        // The token for this node is the operator itself, which helps locate errors.
        this->token = op;
    }

    std::string debugString(int indent = 0) const override {
        std::stringstream ss;
        ss << indentString(indent) << "BinaryOpNode: (" << op.lexeme << ")\n";
        ss << left->debugString(indent + 1);
        ss << right->debugString(indent + 1);
        return ss.str();
    }
};

#endif //CHTL_BINARYOPNODE_H
