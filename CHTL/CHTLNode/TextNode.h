#pragma once

#include "BaseNode.h"
#include <string>

class TextNode : public BaseNode {
public:
    std::string content;

    void accept(AstVisitor& visitor) override {
        visitor.visit(*this);
    }
};
