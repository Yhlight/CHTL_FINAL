#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

class CustomStyleTemplateNode : public BaseNode {
public:
    std::string name;
    std::map<std::string, std::string> properties;
    std::vector<std::string> valuelessProperties;

    void accept(class AstVisitor& visitor) override;

    std::unique_ptr<BaseNode> clone() const override {
        auto node = std::make_unique<CustomStyleTemplateNode>();
        node->name = this->name;
        node->properties = this->properties;
        node->valuelessProperties = this->valuelessProperties;
        return node;
    }
};
