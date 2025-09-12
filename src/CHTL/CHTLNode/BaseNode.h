#pragma once

#include <memory>
#include <vector>

namespace CHTL {

// Forward declaration for the visitor pattern, which will be used later.
class AstVisitor;

class BaseNode {
public:
    virtual ~BaseNode() = default;

    // The accept method for the visitor pattern.
    // It's pure virtual to make this class abstract.
    // We will uncomment this when we implement the visitor.
    // virtual void accept(AstVisitor& visitor) = 0;
};

} // namespace CHTL
