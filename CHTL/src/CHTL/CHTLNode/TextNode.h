#pragma once

#include "BaseNode.h"
#include <string>
#include <memory>

namespace CHTL {

class TextNode : public BaseNode {
public:
    explicit TextNode(const std::string& text) : m_text(text) {}

    NodeType getType() const override { return NodeType::Text; }

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<TextNode>(m_text);
    }

    const std::string& getText() const { return m_text; }

private:
    std::string m_text;
};

} // namespace CHTL
