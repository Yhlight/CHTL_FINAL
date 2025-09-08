#pragma once

#include "BaseNode.h"
#include <string>
#include <memory>

class OriginNode : public BaseNode {
public:
    std::string content;
    std::string name_to_lookup; // For usage like '[Origin] @Html myFile;'

    void accept(class AstVisitor& visitor) override;

    std::unique_ptr<BaseNode> clone() const override {
        auto node = std::make_unique<OriginNode>();
        node->content = this->content;
        node->name_to_lookup = this->name_to_lookup;
        return node;
    }
};
