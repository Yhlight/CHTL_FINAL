#ifndef CHTL_ROOT_NODE_H
#define CHTL_ROOT_NODE_H

#include "BaseNode.h"
#include <vector>
#include <memory>

namespace CHTL {

// Represents the root of a parsed CHTL document.
class RootNode : public Node {
public:
    RootNode() = default;

    NodeType getType() const override { return NodeType::Root; }

    std::vector<std::unique_ptr<Node>> children_;
};

} // namespace CHTL

#endif // CHTL_ROOT_NODE_H
