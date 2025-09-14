#ifndef CHTL_RAW_SCRIPT_NODE_H
#define CHTL_RAW_SCRIPT_NODE_H

#include "BaseNode.h"

namespace CHTL {

class RawScriptNode : public Node {
public:
    explicit RawScriptNode(std::string content) : content_(std::move(content)) {}

    NodeType getType() const override { return NodeType::RawScript; }

    std::unique_ptr<Node> clone() const override {
        return std::make_unique<RawScriptNode>(content_);
    }

    const std::string& getContent() const { return content_; }

private:
    std::string content_;
};

} // namespace CHTL

#endif // CHTL_RAW_SCRIPT_NODE_H
