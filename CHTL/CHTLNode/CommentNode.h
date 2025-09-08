#ifndef COMMENT_NODE_H
#define COMMENT_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class CommentNode : public BaseNode {
public:
    CommentNode(const std::string& content, bool isGeneratorComment);

    NodeType getType() const override;
    std::unique_ptr<BaseNode> clone() const override; // Added

    const std::string& getContent() const;
    bool isGeneratorComment() const;

private:
    std::string m_content;
    bool m_isGeneratorComment;
};

} // namespace CHTL

#endif // COMMENT_NODE_H
