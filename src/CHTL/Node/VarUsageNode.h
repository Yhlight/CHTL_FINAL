#ifndef CHTL_VAR_USAGE_NODE_H
#define CHTL_VAR_USAGE_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

// Represents a variable usage, e.g., ThemeColor(tableColor)
class VarUsageNode : public BaseNode {
public:
    std::string groupName; // e.g., "ThemeColor"
    std::string varName;   // e.g., "tableColor"
};

} // namespace CHTL

#endif // CHTL_VAR_USAGE_NODE_H
