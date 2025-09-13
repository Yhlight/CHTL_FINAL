#ifndef CHTL_COMMENT_NODE_H
#define CHTL_COMMENT_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class CommentNode : public Node {
public:
    explicit CommentNode(const std::string& comment) : comment_(comment) {}

    NodeType getType() const override { return NodeType::Comment; }
    const char* getTypeName() const override { return "Comment"; }

    std::unique_ptr<Node> clone() const override {
        return std::make_unique<CommentNode>(comment_);
    }

    std::string comment_;
};

} // namespace CHTL

#endif // CHTL_COMMENT_NODE_H
