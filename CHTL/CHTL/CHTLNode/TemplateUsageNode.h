#pragma once

#include "BaseNode.h"
#include "TemplateDefinitionNode.h" // For TemplateType enum
#include <string>

class TemplateUsageNode : public BaseNode {
public:
    TemplateUsageNode(TemplateType type, const std::string& name)
        : templateType(type), name(name) {}

    void accept(AstVisitor& visitor) override {
        visitor.visit(*this);
    }

    TemplateType templateType;
    std::string name;
};
