#pragma once

#include "Node.h"
#include <string>

class TextNode : public Node {
public:
    TextNode(const std::string& text) : text(text) {}

    void accept(Visitor& visitor) override {
        visitor.visit(this);
    }

    std::string text;
};
