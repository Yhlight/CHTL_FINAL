#pragma once

#include "Node.h"
#include <string>
#include <utility>

namespace CHTL {

class AttributeNode : public Node {
public:
    AttributeNode(std::string key, std::string value)
        : key(std::move(key)), value(std::move(value)) {}

    std::string ToString(int indent = 0) const override {
        return std::string(indent, ' ') + "Attribute( " + key + " = \"" + value + "\" )";
    }

    std::string key;
    std::string value;
};

using AttributePtr = std::shared_ptr<AttributeNode>;
using AttributeList = std::vector<AttributePtr>;

} // namespace CHTL
