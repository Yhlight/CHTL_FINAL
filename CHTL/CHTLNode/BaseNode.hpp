#pragma once

#include <memory>

namespace CHTL {

// Forward declare the visitor
class AstVisitor;

class BaseNode {
public:
    virtual ~BaseNode() = default;
    virtual void accept(AstVisitor& visitor) = 0;
    int line;
};

// Define a shorthand for smart pointers to AST nodes
using NodePtr = std::shared_ptr<BaseNode>;

} // namespace CHTL
