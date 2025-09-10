#pragma once

#include "BaseNode.h"
#include <string>

class OriginNode : public BaseNode {
public:
    std::string type; // e.g., "Html", "Style"
    std::string content;

    OriginNode(const std::string& type, const std::string& content)
        : type(type), content(content) {}
};
