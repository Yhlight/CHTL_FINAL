#pragma once

#include "BaseNode.h"
#include <string>
#include <map>

class StyleTemplateNode : public BaseNode {
public:
    std::string name;
    std::map<std::string, std::string> properties;

    void accept(AstVisitor& visitor) override {
        // Template definitions are not visited directly.
        // They are expanded at usage sites.
    }
};
