#pragma once

#include "BaseNode.h"
#include <string>
#include <map>

class VarTemplateNode : public BaseNode {
public:
    std::string name;
    std::map<std::string, std::string> variables;

    void accept(AstVisitor& visitor) override {
        // Not visited directly
    }

    std::unique_ptr<BaseNode> clone() const override {
        auto node = std::make_unique<VarTemplateNode>();
        node->name = this->name;
        node->variables = this->variables;
        return node;
    }
};
