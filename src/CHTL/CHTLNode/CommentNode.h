#pragma once

#include "BaseNode.h"
#include <string>

class CommentNode : public BaseNode {
public:
    CommentNode(const std::string& content) : content(content) {}

    NodeType getType() const override {
        return NodeType::Comment;
    }

    std::string content;
};
