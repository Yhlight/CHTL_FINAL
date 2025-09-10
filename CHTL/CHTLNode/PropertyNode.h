#pragma once

#include "ExpressionNode.h"
#include <string>
#include <memory>

namespace CHTL {

struct Property {
    std::string name;
    ExpressionNodePtr value;
};

} // namespace CHTL
