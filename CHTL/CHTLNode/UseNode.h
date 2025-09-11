#pragma once

#include "Node.h"
#include "CHTLLexer/Token.h"
#include <string>
#include <vector>

class UseNode : public Node {
public:
    explicit UseNode(std::vector<Token> target) : target(std::move(target)) {}

    void accept(Visitor& visitor) override;

    std::string getTargetAsString() const {
        std::string result;
        for(size_t i = 0; i < target.size(); ++i) {
            result += target[i].value;
            if (i < target.size() - 1) result += " ";
        }
        return result;
    }

    std::vector<Token> target;
};
