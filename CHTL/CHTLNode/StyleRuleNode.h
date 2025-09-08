#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

class StyleRuleNode : public BaseNode {
public:
    std::string selector;
    std::map<std::string, std::string> properties;

    void accept(class AstVisitor& visitor) override;

    std::unique_ptr<BaseNode> clone() const override {
        auto node = std::make_unique<StyleRuleNode>();
        node->selector = this->selector;
        node->properties = this->properties;
        return node;
    }
};
