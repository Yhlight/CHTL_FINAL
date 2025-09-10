#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>

class NamespaceNode : public BaseNode {
public:
    std::string name;
    NodeList children;

    NamespaceNode(const std::string& name) : name(name) {}
};
