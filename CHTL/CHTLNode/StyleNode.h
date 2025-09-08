#pragma once

#include "BaseNode.h"
#include <string>
#include <map>
#include <vector>
#include "StyleRuleNode.h"

class StyleNode : public BaseNode {
public:
    std::map<std::string, std::string> inlineProperties;
    std::vector<std::unique_ptr<StyleRuleNode>> rules;

    void accept(AstVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<BaseNode> clone() const override {
        auto node = std::make_unique<StyleNode>();
        node->inlineProperties = this->inlineProperties;
        for (const auto& rule : this->rules) {
            node->rules.push_back(std::unique_ptr<StyleRuleNode>(static_cast<StyleRuleNode*>(rule->clone().release())));
        }
        return node;
    }
};
