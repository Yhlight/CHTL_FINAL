#pragma once

#include "Node.h"
#include <string>

class ScriptNode : public Node {
public:
    explicit ScriptNode(std::string placeholder) : placeholder(std::move(placeholder)) {}

    void accept(Visitor& visitor) override;

    std::string placeholder;
};
