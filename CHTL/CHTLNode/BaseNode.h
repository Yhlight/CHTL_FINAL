#ifndef CHTL_BASE_NODE_H
#define CHTL_BASE_NODE_H

#include <memory>

// Forward declaration of the visitor
class AstVisitor;

class BaseNode {
public:
    virtual ~BaseNode() = default;
    // The accept method for the visitor pattern.
    // This will be implemented by each concrete node type.
    virtual void accept(AstVisitor& visitor) = 0;
};

#endif // CHTL_BASE_NODE_H
