#pragma once

#include "BaseNode.h"
#include <string>

class TextNode : public BaseNode {
public:
    TextNode(const std::string& text) : text(text) {}

    void accept(AstVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::string text;
};
