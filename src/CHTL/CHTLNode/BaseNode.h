#ifndef CHTL_BASENODE_H
#define CHTL_BASENODE_H

#include <memory>
#include <vector>

namespace CHTL {

// The Visitor pattern requires forward declaring the visitor class.
class NodeVisitor;

// BaseNode is the abstract base class for all nodes in the AST.
class BaseNode {
public:
    virtual ~BaseNode() = default;

    // The 'accept' method is the entry point for the Visitor pattern.
    virtual void accept(NodeVisitor& visitor) = 0;

    // Children are stored in the base node to represent the tree structure.
    void addChild(std::unique_ptr<BaseNode> child) {
        children.push_back(std::move(child));
    }

    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const {
        return children;
    }

protected:
    std::vector<std::unique_ptr<BaseNode>> children;
};

} // namespace CHTL

#endif // CHTL_BASENODE_H
