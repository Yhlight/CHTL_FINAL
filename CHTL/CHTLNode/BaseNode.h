#ifndef BASE_NODE_H
#define BASE_NODE_H

#include <memory>

namespace CHTL {

// Forward declaration for the Visitor pattern.
// The Visitor pattern allows us to add new operations to the AST
// without changing the node classes themselves.
class Visitor;

class BaseNode {
public:
    virtual ~BaseNode() = default;
    virtual void accept(Visitor& visitor) = 0;
    virtual std::unique_ptr<BaseNode> clone() const = 0;
};

} // namespace CHTL

#endif // BASE_NODE_H
