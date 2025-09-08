#pragma once

#include "BaseNode.h"
#include <string>
#include <map>
#include "ExpressionNode.h"

class VarTemplateNode : public BaseNode {
public:
    std::string name;
    std::map<std::string, std::unique_ptr<ExpressionNode>> variables;

    void accept(AstVisitor& visitor) override {
        // Not visited directly
    }

    std::unique_ptr<BaseNode> clone() const override {
        auto node = std::make_unique<VarTemplateNode>();
        node->name = this->name;
        for (const auto& var : this->variables) {
            if (var.second) {
                node->variables[var.first] = std::unique_ptr<ExpressionNode>(static_cast<ExpressionNode*>(var.second->clone().release()));
            }
        }
        return node;
    }
};
