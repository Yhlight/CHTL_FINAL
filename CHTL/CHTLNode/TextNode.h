#pragma once

#include "BaseNode.h"
#include <string>

class TextNode : public BaseNode {
public:
    std::string content;

    TextNode(const std::string& content) : content(content) {}
};
