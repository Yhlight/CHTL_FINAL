#pragma once

#include <string>
#include <memory>
#include <vector>

namespace CHTL {

// Forward declaration
class Visitor;

class Node {
public:
    virtual ~Node() = default;
    virtual std::string toString(int indent = 0) const = 0;
    // virtual void accept(Visitor& visitor) = 0; // For visitor pattern later
};

// Base class for all statements
class Statement : public Node {};

// Base class for all expressions
class Expression : public Node {};

} // namespace CHTL
