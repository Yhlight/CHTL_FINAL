#ifndef CHTL_ORIGIN_USAGE_NODE_H
#define CHTL_ORIGIN_USAGE_NODE_H

#include "BaseNode.h"
#include <string>

// Represents the usage of a named origin block, e.g., `[Origin] @Html myBlock;`
class OriginUsageNode : public BaseNode {
public:
    std::string type;
    std::string name;

    OriginUsageNode(const std::string& t, const std::string& n) : type(t), name(n) {}
};

#endif // CHTL_ORIGIN_USAGE_NODE_H
