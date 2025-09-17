#pragma once

#include "BaseNode.h"
#include "Expression/ExprNode.h"
#include <string>
#include <memory>

class PropertyNode : public BaseNode {
public:
    PropertyNode(const std::string& key, std::shared_ptr<ExprNode> value)
        : key(key), value(value) {}

    void accept(AstVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::string key;
    std::shared_ptr<ExprNode> value;
};
