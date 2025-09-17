#pragma once

#include "Node.h"
#include <string>

namespace CHTL {

// Represents @Type Name;
class TemplateUsageNode : public Node {
public:
    std::string templateType; // e.g., "Style", "Element"
    std::string name;

    std::string ToString() const override;
};

} // namespace CHTL
