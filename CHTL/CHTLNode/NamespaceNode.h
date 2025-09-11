#pragma once

#include "Node.h"
#include <string>
#include <vector>

class NamespaceNode : public Node {
public:
    explicit NamespaceNode(std::string name) : name(std::move(name)) {}

    void accept(Visitor& visitor) override;

    void addChild(NodePtr child) {
        children.push_back(std::move(child));
    }

    std::string name;
    NodeList children;
};
