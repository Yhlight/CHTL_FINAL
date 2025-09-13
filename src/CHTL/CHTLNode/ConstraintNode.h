#ifndef CHTL_CONSTRAINT_NODE_H
#define CHTL_CONSTRAINT_NODE_H

#include "BaseNode.h"
#include "../CHTLLexer/Token.h"
#include <vector>
#include <memory>

namespace CHTL {

struct ConstraintNode : public Node {
    std::vector<Token> constraint_tokens;

    ConstraintNode(std::vector<Token> tokens)
        : constraint_tokens(std::move(tokens)) {}

    NodeType getType() const override { return NodeType::Constraint; }

    std::unique_ptr<Node> clone() const override {
        // The tokens themselves are copyable
        return std::make_unique<ConstraintNode>(this->constraint_tokens);
    }
};

} // namespace CHTL

#endif // CHTL_CONSTRAINT_NODE_H
