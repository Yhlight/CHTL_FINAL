#ifndef COMMENT_NODE_H
#define COMMENT_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class CommentNode : public BaseNode {
public:
    std::string text;

    CommentNode(const std::string& text);

    void accept(Visitor& visitor) override;
    std::unique_ptr<BaseNode> clone() const override;
};

} // namespace CHTL

#endif // COMMENT_NODE_H
