#ifndef CHTL_ORIGIN_NODE_H
#define CHTL_ORIGIN_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class OriginNode : public Node {
public:
    OriginNode(const std::string& type, const std::string& content, const std::string& name = "")
        : type_(type), content_(content), name_(name) {}

    NodeType getType() const override { return NodeType::Origin; }

    std::unique_ptr<Node> clone() const override {
        return std::make_unique<OriginNode>(type_, content_, name_);
    }

    std::string type_;
    std::string content_;
    std::string name_; // For named origin blocks
};

} // namespace CHTL

#endif // CHTL_ORIGIN_NODE_H
