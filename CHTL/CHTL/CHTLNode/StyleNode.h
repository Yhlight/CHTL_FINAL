#pragma once

#include "BaseNode.h"
#include "PropertyNode.h"
#include "RuleNode.h"
#include <vector>
#include <memory>

class StyleNode : public BaseNode {
public:
    StyleNode() = default;

    void accept(AstVisitor& visitor) override {
        visitor.visit(*this);
    }

    void addProperty(std::shared_ptr<PropertyNode> prop) {
        properties.push_back(prop);
    }

    void addRule(std::shared_ptr<RuleNode> rule) {
        rules.push_back(rule);
    }

    // For inline styles
    std::vector<std::shared_ptr<PropertyNode>> properties;
    // For global styles
    std::vector<std::shared_ptr<RuleNode>> rules;
};
