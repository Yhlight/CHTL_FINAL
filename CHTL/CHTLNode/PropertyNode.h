#pragma once

#include "ExpressionNode.h"
#include <string>
#include <memory>

namespace CHTL {

// Represents a property, which can be a standard key-value pair
// or a valueless property for specialization in [Custom] blocks.
struct Property {
    std::string name;
    ExpressionNodePtr value; // Can be nullptr for valueless properties
};

} // namespace CHTL
