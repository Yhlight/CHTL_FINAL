#pragma once

#include "Node.h"
#include <vector>
#include <memory>

namespace CHTL {

// Represents a simple block of statements, used as the body
// of an element template.
class ElementBlockNode : public Node {
public:
    std::vector<std::unique_ptr<Node>> statements;

    std::string ToString() const override;
};

} // namespace CHTL
