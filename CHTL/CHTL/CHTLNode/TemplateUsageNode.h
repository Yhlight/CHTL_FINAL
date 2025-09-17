#pragma once

#include "BaseNode.h"
#include "TemplateDefinitionNode.h" // For TemplateType enum
#include <string>
#include <vector>
#include <memory>

class TemplateUsageNode : public BaseNode {
public:
    TemplateUsageNode(TemplateType type, const std::string& name)
        : templateType(type), name(name) {}

    void accept(AstVisitor& visitor) override {
        visitor.visit(*this);
    }

    // This will hold the nodes from the specialization block, e.g., { color: red; }
    std::vector<std::shared_ptr<BaseNode>> specializationBody;
    TemplateType templateType;
    std::string name;
};
