#pragma once

#include <memory>
#include <vector>
#include <string>

// Forward declaration for the Visitor pattern
class AstVisitor;

class BaseNode {
public:
    virtual ~BaseNode() = default;
    // The visitor pattern will be useful for the generator
    // virtual void accept(AstVisitor& visitor) = 0;
};

// Using smart pointers for memory management of AST nodes
using AstNodePtr = std::unique_ptr<BaseNode>;
using NodeList = std::vector<AstNodePtr>;
