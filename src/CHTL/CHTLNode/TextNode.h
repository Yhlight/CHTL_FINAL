#ifndef CHTL_TEXT_NODE_H
#define CHTL_TEXT_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class TextNode : public Node {
public:
    explicit TextNode(const std::string& text) : text_(text) {}

    NodeType getType() const override { return NodeType::Text; }
    const char* getTypeName() const override { return "Text"; }

    std::unique_ptr<Node> clone() const override {
        return std::make_unique<TextNode>(text_);
    }

    std::string text_;
};

} // namespace CHTL

#endif // CHTL_TEXT_NODE_H
