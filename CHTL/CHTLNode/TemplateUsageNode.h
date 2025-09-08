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
};
