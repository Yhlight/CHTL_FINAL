#ifndef CHTL_ORIGIN_NODE_H
#define CHTL_ORIGIN_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class OriginNode : public Node {
public:
    OriginNode(const std::string& type, const std::string& content)
        : type_(type), content_(content) {}

    NodeType getType() const override { return NodeType::Origin; }

    std::unique_ptr<Node> clone() const override {
        return std::make_unique<OriginNode>(type_, content_);
    }

    std::string type_;
    std::string content_;
};

} // namespace CHTL

#endif // CHTL_ORIGIN_NODE_H
