#pragma once

#include "BaseNode.h"
#include <string>
#include <memory>

namespace CHTL {

class CommentNode : public BaseNode {
public:
    explicit CommentNode(const std::string& text) : m_text(text) {}

    NodeType getType() const override { return NodeType::Comment; }

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<CommentNode>(m_text);
    }

    const std::string& getText() const { return m_text; }

private:
    std::string m_text;
};

} // namespace CHTL
