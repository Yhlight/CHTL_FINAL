#ifndef CHTL_EXPRESSIONNODE_H
#define CHTL_EXPRESSIONNODE_H

#include "BaseNode.h"

// Base class for all expression nodes (e.g., literals, binary operations).
// Itself inherits from BaseNode so expressions can be part of the main AST.
class ExpressionNode : public BaseNode {
public:
    // The debugString method is inherited and must be implemented by concrete expression classes.
};

#endif //CHTL_EXPRESSIONNODE_H
