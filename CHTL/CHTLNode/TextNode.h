#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

// Represents a text node in the CHTL document, like `text { "Hello, World!" }`.
class TextNode : public BaseNode {
public:
    std::string value;

    TextNode(const std::string& val) : value(val) {}

    std::string toString() const override {
        return "TextNode(value: \"" + value + "\")";
    }
};

} // namespace CHTL
