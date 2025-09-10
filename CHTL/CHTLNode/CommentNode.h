#pragma once

#include "Node.h"
#include <string>

class CommentNode : public Node {
public:
    CommentNode(const std::string& text, bool isGeneratorComment)
        : text(text), isGeneratorComment(isGeneratorComment) {}

    void accept(Visitor& visitor) override {
        visitor.visit(this);
    }

    std::string text;
    bool isGeneratorComment;
};
