#pragma once

#include "Node.h"

namespace CHTL {

class ExpressionNode : public Node {
public:
    // Base class for all expression nodes
};

using ExpressionPtr = std::shared_ptr<ExpressionNode>;

} // namespace CHTL
