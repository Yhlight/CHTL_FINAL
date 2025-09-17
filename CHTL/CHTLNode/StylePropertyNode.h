#pragma once

#include "Node.h"
#include <string>

namespace CHTL {

// Represents a single CSS property, e.g., `width: 100px;`
class StylePropertyNode : public Node {
public:
    std::string key;
    std::string value;

    std::string ToString() const override;
};

} // namespace CHTL
