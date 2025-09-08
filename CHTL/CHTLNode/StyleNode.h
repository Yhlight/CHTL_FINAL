#pragma once

#include "BaseNode.h"
#include <string>
#include <map>

class StyleNode : public BaseNode {
public:
    std::map<std::string, std::string> properties;

    void accept(AstVisitor& visitor) override {
        visitor.visit(*this);
    }
};
