#pragma once

#include "BaseNode.h"
#include <vector>
#include <memory>

class StyleNode : public BaseNode {
public:
    StyleNode() = default;

    void accept(AstVisitor& visitor) override {
        visitor.visit(*this);
    }

    void addChild(std::shared_ptr<BaseNode> child) {
        children.push_back(child);
    }

    // A style block can contain properties, rules, or template usages.
    // The template resolver will expand usages into properties and rules.
    std::vector<std::shared_ptr<BaseNode>> children;
};
