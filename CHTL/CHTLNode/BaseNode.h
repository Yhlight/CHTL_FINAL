#ifndef CHTL_BASENODE_H
#define CHTL_BASENODE_H

#include <memory>
#include <vector>

namespace CHTL {

class BaseNode {
public:
    virtual ~BaseNode() = default;
    // A virtual print method for debugging the AST is useful.
    virtual void print(int indent = 0) const = 0;
};

// Using smart pointers for AST nodes to manage memory automatically.
using NodePtr = std::shared_ptr<BaseNode>;
using NodeList = std::vector<NodePtr>;

} // namespace CHTL

#endif // CHTL_BASENODE_H
