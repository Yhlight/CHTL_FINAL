#pragma once

#include "BaseNode.h"
#include <string>
#include <memory>

class OriginNode : public BaseNode {
public:
    std::string content;

    void accept(class AstVisitor& visitor) override;

    std::unique_ptr<BaseNode> clone() const override {
        auto node = std::make_unique<OriginNode>();
        node->content = this->content;
        return node;
    }
};
