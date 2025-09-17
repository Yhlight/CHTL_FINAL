#pragma once

#include "BaseNode.h"
#include <vector>
#include <memory>

class RootNode : public BaseNode {
public:
    RootNode() = default;

    void accept(AstVisitor& visitor) override {
        visitor.visit(*this);
    }

    void addChild(std::shared_ptr<BaseNode> child) {
        children.push_back(child);
    }

    std::vector<std::shared_ptr<BaseNode>> children;
};
