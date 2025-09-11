#pragma once

#include "Node.h"
#include <string>
#include <utility>

namespace CHTL {

class TextNode : public Node {
public:
    TextNode(std::string text) : text(std::move(text)) {}

    std::string ToString(int indent = 0) const override {
        return std::string(indent, ' ') + "Text( \"" + text + "\" )";
    }

    NodePtr clone() const override {
        return std::make_shared<TextNode>(text);
    }

    std::string text;
};

} // namespace CHTL
