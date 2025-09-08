#pragma once

#include <string>
#include <vector>

struct AttributeNode {
    std::string key;
    std::string value;
};

using AttributeList = std::vector<AttributeNode>;
