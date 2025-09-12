#ifndef CHTL_COMMENT_NODE_H
#define CHTL_COMMENT_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class CommentNode : public Node {
public:
    explicit CommentNode(const std::string& comment) : comment_(comment) {}

    NodeType getType() const override { return NodeType::Comment; }

    std::string comment_;
};

} // namespace CHTL

#endif // CHTL_COMMENT_NODE_H
