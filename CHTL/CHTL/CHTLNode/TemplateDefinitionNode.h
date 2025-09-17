#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

enum class TemplateType {
    STYLE,
    ELEMENT,
    VAR
};

class TemplateDefinitionNode : public BaseNode {
public:
    TemplateDefinitionNode(TemplateType type, const std::string& name)
        : templateType(type), name(name) {}

    void accept(AstVisitor& visitor) override {
        visitor.visit(*this);
    }

    void addChild(std::shared_ptr<BaseNode> child) {
        body.push_back(child);
    }

    TemplateType templateType;
    std::string name;
    std::vector<std::shared_ptr<BaseNode>> body;
};
