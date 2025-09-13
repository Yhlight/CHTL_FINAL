#ifndef CHTL_SCRIPT_BLOCK_NODE_H
#define CHTL_SCRIPT_BLOCK_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class ScriptBlockNode : public Node {
public:
    explicit ScriptBlockNode(const std::string& content) : content_(content) {}

    NodeType getType() const override { return NodeType::ScriptBlock; }

    std::unique_ptr<Node> clone() const override {
        return std::make_unique<ScriptBlockNode>(content_);
    }

    std::string content_;
};

} // namespace CHTL

#endif // CHTL_SCRIPT_BLOCK_NODE_H
