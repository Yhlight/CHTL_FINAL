#pragma once

#include "Node.h"
#include "CHTLLexer/Token.h"
#include <string>
#include <vector>

class ConstraintNode : public Node {
public:
    explicit ConstraintNode(std::vector<Token> targets) : constrained_targets(std::move(targets)) {}

    void accept(Visitor& visitor) override;

    std::vector<Token> constrained_targets;
};
