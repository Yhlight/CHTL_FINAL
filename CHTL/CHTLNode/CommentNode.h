#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

enum class CommentType {
    Standard, // // or /* */
    Generator // --
};

class CommentNode : public BaseNode {
public:
    explicit CommentNode(std::string text, CommentType commentType)
        : m_text(std::move(text)), m_commentType(commentType) {}

    NodeType GetType() const override { return NodeType::Comment; }
    const std::string& GetText() const { return m_text; }
    CommentType GetCommentType() const { return m_commentType; }

private:
    std::string m_text;
    CommentType m_commentType;
};

} // namespace CHTL
