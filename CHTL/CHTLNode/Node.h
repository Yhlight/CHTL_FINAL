#pragma once

#include <string>
#include <memory>
#include <vector>

namespace CHTL {

class Node {
public:
    virtual ~Node() = default;

    // For debugging and verification
    virtual std::string ToString() const = 0;
};

// Program is the root node of every AST our parser produces.
class Program : public Node {
public:
    std::string ToString() const override;

    std::vector<std::unique_ptr<Node>> statements;
};

} // namespace CHTL
