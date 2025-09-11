#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

class TextNode : public BaseNode {
public:
    explicit TextNode(std::string text) : m_text(std::move(text)) {}

    NodeType GetType() const override { return NodeType::Text; }
    const std::string& GetText() const { return m_text; }

    NodePtr Clone() const override {
        return std::make_shared<TextNode>(m_text);
    }

private:
    std::string m_text;
};

} // namespace CHTL
