#pragma once

#include "BaseNode.h"
#include <string>
#include <map>
#include "ExpressionNode.h"

class StyleTemplateNode : public BaseNode {
public:
    std::string name;
    std::map<std::string, std::unique_ptr<ExpressionNode>> properties;

    void accept(AstVisitor& visitor) override {
        // Not visited directly
    }

    std::unique_ptr<BaseNode> clone() const override {
        auto node = std::make_unique<StyleTemplateNode>();
        node->name = this->name;
        for (const auto& prop : this->properties) {
            if (prop.second) {
                node->properties[prop.first] = std::unique_ptr<ExpressionNode>(static_cast<ExpressionNode*>(prop.second->clone().release()));
            }
        }
        return node;
    }
};
