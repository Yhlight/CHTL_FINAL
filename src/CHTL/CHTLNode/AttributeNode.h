#ifndef CHTL_ATTRIBUTE_NODE_H
#define CHTL_ATTRIBUTE_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class AttributeNode { // Does not need to inherit from Node, as it's not a child in the same way
public:
    AttributeNode(const std::string& key, const std::string& value) : key_(key), value_(value) {}

    std::string key_;
    std::string value_;
};

} // namespace CHTL

#endif // CHTL_ATTRIBUTE_NODE_H
