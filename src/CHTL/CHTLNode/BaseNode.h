#ifndef BASE_NODE_H
#define BASE_NODE_H

#include <memory>
#include <vector>

// Forward declaration for the Visitor pattern
class Visitor;

class BaseNode {
public:
    virtual ~BaseNode() = default;
    virtual void accept(Visitor& visitor) = 0;
    virtual std::unique_ptr<BaseNode> clone() const = 0;
};

#endif // BASE_NODE_H
