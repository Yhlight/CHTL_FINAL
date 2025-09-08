#pragma once

#include "BaseNode.h"
#include <string>
#include <memory>

class ScriptNode : public BaseNode {
public:
    std::string content;

    void accept(AstVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<BaseNode> clone() const override {
        auto node = std::make_unique<ScriptNode>();
        node->content = this->content;
        return node;
    }
};
