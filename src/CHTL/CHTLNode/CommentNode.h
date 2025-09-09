#ifndef COMMENT_NODE_H
#define COMMENT_NODE_H

#include "BaseNode.h"
#include <string>

class CommentNode : public BaseNode {
public:
    CommentNode(const std::string& text);

    void accept(Visitor& visitor) override;
    std::unique_ptr<BaseNode> clone() const override;

    const std::string& getText() const;

private:
    std::string text;
};

#endif // COMMENT_NODE_H
