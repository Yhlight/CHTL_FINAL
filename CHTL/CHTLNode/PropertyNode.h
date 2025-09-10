#pragma once

#include "Node.h"
#include <string>

class PropertyNode : public Node {
public:
    PropertyNode(const std::string& name, const std::string& value)
        : name(name), value(value) {}

    void accept(Visitor& visitor) override {
        visitor.visit(this);
    }

    std::string name;
    std::string value; // For now, value is a simple string. This will be expanded later.
};
