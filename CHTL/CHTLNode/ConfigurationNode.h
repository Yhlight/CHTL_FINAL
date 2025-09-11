#pragma once

#include "Node.h"
#include "PropertyNode.h"
#include <vector>
#include <memory>

class ConfigurationNode : public Node {
public:
    ConfigurationNode() = default;

    void accept(Visitor& visitor) override { visitor.visit(this); }

    std::vector<std::unique_ptr<PropertyNode>> settings;
};
