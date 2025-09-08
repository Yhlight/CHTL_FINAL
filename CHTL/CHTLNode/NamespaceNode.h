#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

class NamespaceNode : public BaseNode {
public:
    std::string name;
    std::vector<std::unique_ptr<BaseNode>> children;

    void accept(AstVisitor& visitor) override {
        // This will require a new visit method on the AstVisitor if we need to generate from it,
        // but since namespaces are resolved by the parser, the generator may not need to visit it.
        // For now, we leave it empty.
    }

    std::unique_ptr<BaseNode> clone() const override {
        auto node = std::make_unique<NamespaceNode>();
        node->name = this->name;
        for (const auto& child : this->children) {
            auto clonedChild = child->clone();
            clonedChild->parent = node.get();
            node->children.push_back(std::move(clonedChild));
        }
        return node;
    }
};
