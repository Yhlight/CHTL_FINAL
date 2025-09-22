#pragma once

#include <string>
#include <memory>

namespace CHTL {

// Base class for all nodes in the Abstract Syntax Tree (AST).
// It's an abstract class that defines the common interface for all node types.
class BaseNode {
public:
    // Virtual destructor is crucial for a polymorphic base class
    // to ensure that derived class destructors are called correctly.
    virtual ~BaseNode() = default;

    // A pure virtual function for debugging, forcing derived classes
    // to implement a way to represent themselves as a string.
    virtual std::string toString() const = 0;
};

} // namespace CHTL
