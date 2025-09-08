#pragma once

#include "BaseNode.h"
#include <string>
#include <map>

class StyleTemplateNode : public BaseNode {
public:
    std::string name;
    std::map<std::string, std::string> properties;

    void accept(AstVisitor& visitor) override {
        // Not visited directly
    }

    std::unique_ptr<BaseNode> clone() const override {
        auto node = std::make_unique<StyleTemplateNode>();
        node->name = this->name;
        node->properties = this->properties;
        return node;
    }
};
