#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

class CustomElementNode : public BaseNode {
public:
    std::string name;
    std::vector<std::unique_ptr<BaseNode>> children;

    void accept(AstVisitor& visitor) override {
        // Custom definitions are not visited directly.
    }

    std::unique_ptr<BaseNode> clone() const override {
        auto node = std::make_unique<CustomElementNode>();
        node->name = this->name;
        for (const auto& child : this->children) {
            node->children.push_back(child->clone());
        }
        return node;
    }
};
