#pragma once

#include <memory>
#include <vector>

enum class NodeType {
    Element,
    Text,
    Comment,
    // Other node types will be added later
};

class BaseNode {
public:
    virtual ~BaseNode() = default;
    virtual NodeType getType() const = 0;
};

// Use smart pointers for memory management of nodes
using NodePtr = std::unique_ptr<BaseNode>;
using NodeList = std::vector<NodePtr>;
