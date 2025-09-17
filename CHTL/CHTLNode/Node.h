#pragma once

#include <string>
#include <memory>
#include <vector>

// Forward declaration of Node types
class Statement;
class Expression;

// The base class for all nodes in the Abstract Syntax Tree (AST).
class Node {
public:
    virtual ~Node() = default;

    // A virtual method for debugging, to get a string representation of the node.
    virtual std::string toString() const = 0;
};

// The base class for all statement nodes (e.g., an element definition).
// Statements do not produce values.
class Statement : public Node {
public:
    // A dummy implementation for now.
    std::string toString() const override { return "[Statement]"; }
};

// The base class for all expression nodes (e.g., an attribute value).
// Expressions produce values.
class Expression : public Node {
public:
    // A dummy implementation for now.
    std::string toString() const override { return "[Expression]"; }
};

// The root node of the AST, representing an entire CHTL file.
class Program : public Node {
public:
    std::vector<std::shared_ptr<Statement>> statements;

    std::string toString() const override;
};
