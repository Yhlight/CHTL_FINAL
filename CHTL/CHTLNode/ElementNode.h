#pragma once

#include "Node.h"
#include "PropertyNode.h"
#include <string>
#include <vector>
#include <memory>

class ElementNode : public Node {
public:
    ElementNode(const std::string& tagName) : tagName(tagName) {}

    void accept(Visitor& visitor) override {
        visitor.visit(this);
    }

    void addChild(NodePtr child) {
        children.push_back(std::move(child));
    }

    std::string tagName;
    NodeList children;
};
