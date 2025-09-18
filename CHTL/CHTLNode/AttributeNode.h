#ifndef ATTRIBUTE_NODE_H
#define ATTRIBUTE_NODE_H

#include <string>

namespace CHTL {

// Represents a single attribute of an HTML element, e.g., id="box".
struct AttributeNode {
    std::string key;
    std::string value;
};

} // namespace CHTL

#endif // ATTRIBUTE_NODE_H
