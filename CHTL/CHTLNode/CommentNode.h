#pragma once

#include "BaseNode.h"
#include <string>

class CommentNode : public BaseNode {
public:
    std::string content;

    CommentNode(const std::string& content) : content(content) {}
};
