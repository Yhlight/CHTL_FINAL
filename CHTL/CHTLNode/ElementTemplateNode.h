#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

class ElementTemplateNode : public BaseNode {
public:
    std::string name;
    std::vector<std::unique_ptr<BaseNode>> children;

    void accept(AstVisitor& visitor) override {
        // Template definitions are not visited directly.
        // They are expanded at usage sites.
    }
};
