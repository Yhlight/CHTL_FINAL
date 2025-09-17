#ifndef CHTL_BASE_NODE_H
#define CHTL_BASE_NODE_H

#include <memory>
#include <string>
#include <vector>

namespace CHTL {

class BaseNode {
public:
    virtual ~BaseNode() = default;
    // A virtual method for the Visitor pattern can be added here later.
    // virtual void accept(class Visitor& visitor) = 0;
};

} // namespace CHTL

#endif // CHTL_BASE_NODE_H
