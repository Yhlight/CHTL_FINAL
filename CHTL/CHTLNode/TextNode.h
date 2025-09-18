#ifndef TEXT_NODE_H
#define TEXT_NODE_H

#include "BaseNode.h"
#include "Visitor.h"
#include <string>

namespace CHTL {

// Represents a text node, e.g., text { "Hello, World" }
class TextNode : public BaseNode {
public:
    explicit TextNode(const std::string& text) : text(text) {}

    void accept(Visitor& visitor) override {
        visitor.visit(*this);
    }

    std::string text;
};

} // namespace CHTL

#endif // TEXT_NODE_H
