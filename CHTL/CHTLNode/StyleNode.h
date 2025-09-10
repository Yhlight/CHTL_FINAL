#pragma once

#include "Node.h"
#include "PropertyNode.h"
#include <vector>
#include <memory>

class StyleNode : public Node {
public:
    StyleNode() = default;

    void accept(Visitor& visitor) override {
        visitor.visit(this);
    }

    void addChild(NodePtr child) {
        children.push_back(std::move(child));
    }

    NodeList children;
};
