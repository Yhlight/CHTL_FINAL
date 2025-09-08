#pragma once

#include "BaseNode.h"
#include <string>
#include <map>
#include <vector>
#include "StyleRuleNode.h"
#include "ExpressionNode.h"

class StyleNode : public BaseNode {
public:
    std::map<std::string, std::unique_ptr<ExpressionNode>> inlineProperties;
    std::vector<std::unique_ptr<StyleRuleNode>> rules;

    void accept(AstVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<BaseNode> clone() const override {
        auto node = std::make_unique<StyleNode>();
        for (const auto& prop : this->inlineProperties) {
            node->inlineProperties[prop.first] = std::unique_ptr<ExpressionNode>(static_cast<ExpressionNode*>(prop.second->clone().release()));
        }
        for (const auto& rule : this->rules) {
            node->rules.push_back(std::unique_ptr<StyleRuleNode>(static_cast<StyleRuleNode*>(rule->clone().release())));
        }
        return node;
    }
};
