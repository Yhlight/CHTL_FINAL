#ifndef CHTL_NAMESPACE_NODE_H
#define CHTL_NAMESPACE_NODE_H

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class NamespaceNode : public Node {
public:
    explicit NamespaceNode(const std::string& name) : name_(name) {}

    NodeType getType() const override { return NodeType::Namespace; }

    std::string name_;
    std::vector<std::unique_ptr<Node>> children_;

    std::unique_ptr<Node> clone() const override {
        auto node = std::make_unique<NamespaceNode>(name_);
        for (const auto& child : children_) {
            node->children_.push_back(child->clone());
        }
        return node;
    }
};

} // namespace CHTL

#endif // CHTL_NAMESPACE_NODE_H
