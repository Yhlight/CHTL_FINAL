#pragma once

#include "Node.h"
#include "ExprNode.h"
#include <string>
#include <memory>

class PropertyNode : public Node {
public:
    PropertyNode(const std::string& name, ExprNodePtr value)
        : name(name), value(std::move(value)) {}

    void accept(Visitor& visitor) override {
        visitor.visit(this);
    }

    std::string name;
    ExprNodePtr value;
};
