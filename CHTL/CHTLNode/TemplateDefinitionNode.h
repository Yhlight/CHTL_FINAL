#pragma once

#include "Node.h"
#include <string>
#include <memory>

namespace CHTL {

// Represents [Template] @Type Name { ... }
class TemplateDefinitionNode : public Node {
public:
    std::string templateType; // e.g., "Style", "Element", "Var"
    std::string name;
    std::unique_ptr<Node> body;

    std::string ToString() const override;
};

} // namespace CHTL
