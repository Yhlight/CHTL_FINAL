#pragma once

#include "BaseNode.h"
#include <string>
#include <map>

class VarTemplateNode : public BaseNode {
public:
    std::string name;
    std::map<std::string, std::string> variables;

    void accept(AstVisitor& visitor) override {
        // Template definitions are not visited directly.
    }
};
