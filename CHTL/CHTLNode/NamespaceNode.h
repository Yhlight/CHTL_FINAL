#ifndef NAMESPACE_NODE_H
#define NAMESPACE_NODE_H

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class NamespaceNode : public BaseNode {
public:
    std::string name;
    std::vector<std::unique_ptr<BaseNode>> children;

    void accept(Visitor& visitor) override {
        // visitor.visit(*this);
    }

    std::unique_ptr<BaseNode> clone() const override {
        auto new_node = std::make_unique<NamespaceNode>();
        new_node->name = this->name;
        for (const auto& child : this->children) {
            new_node->children.push_back(child->clone());
        }
        return new_node;
    }
};

} // namespace CHTL

#endif // NAMESPACE_NODE_H
