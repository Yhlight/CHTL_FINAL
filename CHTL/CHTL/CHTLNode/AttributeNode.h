#pragma once

#include "BaseNode.h"
#include <string>

class AttributeNode : public BaseNode {
public:
    AttributeNode(const std::string& key, const std::string& value)
        : key(key), value(value) {}

    void accept(AstVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::string key;
    std::string value;
};
