#ifndef COMMENT_NODE_H
#define COMMENT_NODE_H

#include "BaseNode.h"
#include <string>
#include <memory>

namespace CHTL {

class CommentNode : public BaseNode {
public:
    std::string comment;

    void accept(Visitor& visitor) override;
    std::unique_ptr<BaseNode> clone() const override;
};

}

#endif //COMMENT_NODE_H
