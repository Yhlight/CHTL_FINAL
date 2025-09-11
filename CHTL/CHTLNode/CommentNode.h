#ifndef CHTL_COMMENT_NODE_H
#define CHTL_COMMENT_NODE_H

#include "BaseNode.h"
#include <string>

class CommentNode : public BaseNode {
public:
    std::string comment;

    void accept(AstVisitor& visitor) override;
};

#endif // CHTL_COMMENT_NODE_H
