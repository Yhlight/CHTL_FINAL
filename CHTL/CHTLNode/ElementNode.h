#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

class ElementNode : public BaseNode {
public:
    std::string tagName;
    std::map<std::string, std::string> attributes;
    std::vector<std::unique_ptr<BaseNode>> children;

    void accept(AstVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<BaseNode> clone() const override {
        auto node = std::make_unique<ElementNode>();
        node->tagName = this->tagName;
        node->attributes = this->attributes;
        for (const auto& child : this->children) {
            auto clonedChild = child->clone();
            clonedChild->parent = node.get();
            node->children.push_back(std::move(clonedChild));
        }
        return node;
    }
};
