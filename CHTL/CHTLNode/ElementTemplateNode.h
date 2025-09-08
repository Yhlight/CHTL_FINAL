#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

class ElementTemplateNode : public BaseNode {
public:
    std::string name;
    std::vector<std::unique_ptr<BaseNode>> children;

    void accept(AstVisitor& visitor) override {
        // Not visited directly
    }

    std::unique_ptr<BaseNode> clone() const override {
        auto node = std::make_unique<ElementTemplateNode>();
        node->name = this->name;
        for (const auto& child : this->children) {
            auto clonedChild = child->clone();
            clonedChild->parent = node.get();
            node->children.push_back(std::move(clonedChild));
        }
        return node;
    }
};
