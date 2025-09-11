#pragma once

#include <string>
#include <memory>
#include <vector>

namespace CHTL {

class Node {
public:
    virtual ~Node() = default;
    virtual std::string ToString(int indent = 0) const = 0;
};

// Using shared_ptr for automatic memory management of nodes
using NodePtr = std::shared_ptr<Node>;
using NodeList = std::vector<NodePtr>;

} // namespace CHTL
