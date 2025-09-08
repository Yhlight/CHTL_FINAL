#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

// Represents a text literal within the document, e.g., the content inside a `text { ... }` block.
class TextNode : public BaseNode {
public:
    explicit TextNode(const std::string& text) : m_text(text) {}

    NodeType getType() const override { return NodeType::Text; }

    const std::string& getText() const { return m_text; }

private:
    std::string m_text;
};

} // namespace CHTL
