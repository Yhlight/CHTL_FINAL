#pragma once

#include "BaseNode.h"
#include <string>

class OriginNode : public BaseNode {
public:
    std::string type;
    std::string rawContent;

    void accept(AstVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<BaseNode> clone() const override {
        auto node = std::make_unique<OriginNode>();
        node->type = this->type;
        node->rawContent = this->rawContent;
        return node;
    }
};
