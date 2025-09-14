#ifndef CHTL_RAW_STYLE_NODE_H
#define CHTL_RAW_STYLE_NODE_H

#include "BaseNode.h"

namespace CHTL {

class RawStyleNode : public Node {
public:
    explicit RawStyleNode(std::string content) : content_(std::move(content)) {}

    NodeType getType() const override { return NodeType::RawStyle; }

    std::unique_ptr<Node> clone() const override {
        return std::make_unique<RawStyleNode>(content_);
    }

    const std::string& getContent() const { return content_; }

private:
    std::string content_;
};

} // namespace CHTL

#endif // CHTL_RAW_STYLE_NODE_H
