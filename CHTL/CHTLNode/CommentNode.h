#ifndef COMMENT_NODE_H
#define COMMENT_NODE_H

#include "BaseNode.h"
#include "Visitor.h"
#include <string>
#include <memory>

namespace CHTL {

class CommentNode : public BaseNode {
public:
    explicit CommentNode(const std::string& text) : text(text) {}

    void accept(Visitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<CommentNode>(text);
    }

    std::string text;
};

} // namespace CHTL

#endif // COMMENT_NODE_H
