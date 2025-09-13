#ifndef CHTL_ROOT_NODE_H
#define CHTL_ROOT_NODE_H

#include "BaseNode.h"
#include <vector>
#include <memory>

namespace CHTL {

// Represents the root of a parsed CHTL document.
class RootNode : public Node {
public:
    RootNode() = default;

    NodeType getType() const override { return NodeType::Root; }
    const char* getTypeName() const override { return "Root"; }

    std::unique_ptr<Node> clone() const override {
        auto new_node = std::make_unique<RootNode>();
        for (const auto& child : this->children_) {
            new_node->children_.push_back(child->clone());
        }
        return new_node;
    }

    std::vector<std::unique_ptr<Node>> children_;
};

} // namespace CHTL

#endif // CHTL_ROOT_NODE_H
