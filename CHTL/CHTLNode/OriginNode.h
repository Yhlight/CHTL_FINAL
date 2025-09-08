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
};
