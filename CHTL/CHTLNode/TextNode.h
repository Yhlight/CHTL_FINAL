#pragma once

#include "BaseNode.h"
#include <string>

class TextNode : public BaseNode {
public:
    std::string content;

    void accept(AstVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<BaseNode> clone() const override {
        auto node = std::make_unique<TextNode>();
        node->content = this->content;
        return node;
    }
};
