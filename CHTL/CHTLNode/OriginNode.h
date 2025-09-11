#ifndef CHTL_ORIGIN_NODE_H
#define CHTL_ORIGIN_NODE_H

#include "BaseNode.h"
#include <string>

// Represents an [Origin] @Type { ... } block
class OriginNode : public BaseNode {
public:
    std::string type; // e.g., "Html", "Style", "JavaScript"
    std::string content;

    OriginNode(const std::string& t, const std::string& c) : type(t), content(c) {}
};

#endif // CHTL_ORIGIN_NODE_H
