#pragma once

#include "BaseNode.h"
#include <string>

class TextNode : public BaseNode {
public:
    TextNode(const std::string& content) : content(content) {}

    NodeType getType() const override {
        return NodeType::Text;
    }

    std::string content;
};
