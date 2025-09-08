#pragma once

#include "BaseNode.h"
#include <string>

enum class TemplateType {
    STYLE,
    ELEMENT
};

class TemplateUsageNode : public BaseNode {
public:
    TemplateType type;
    std::string name;

    void accept(AstVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<BaseNode> clone() const override {
        auto node = std::make_unique<TemplateUsageNode>();
        node->type = this->type;
        node->name = this->name;
        return node;
    }
};
