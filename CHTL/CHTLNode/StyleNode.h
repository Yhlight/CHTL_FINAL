#pragma once

#include "BaseNode.h"
#include <string>
#include <map>

class StyleNode : public BaseNode {
public:
    std::map<std::string, std::string> properties;

    void accept(AstVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<BaseNode> clone() const override {
        auto node = std::make_unique<StyleNode>();
        node->properties = this->properties;
        return node;
    }
};
