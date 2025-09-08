#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

class CustomStyleNode : public BaseNode {
public:
    std::string name;
    std::map<std::string, std::string> properties;
    std::vector<std::string> valuelessProperties;

    void accept(AstVisitor& visitor) override {
        // Custom definitions are not visited directly.
    }
};
