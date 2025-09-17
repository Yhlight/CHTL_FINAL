#pragma once

#include "BaseNode.h"
#include "PropertyNode.h"
#include <string>
#include <vector>
#include <memory>

// Represents a CSS rule, e.g., ".my-class { ... }"
class RuleNode : public BaseNode {
public:
    RuleNode(const std::string& selector) : selector(selector) {}

    void accept(AstVisitor& visitor) override {
        visitor.visit(*this); // <-- THE FIX
    }

    void addProperty(std::shared_ptr<PropertyNode> prop) {
        properties.push_back(prop);
    }

    std::string selector;
    std::vector<std::shared_ptr<PropertyNode>> properties;
};
