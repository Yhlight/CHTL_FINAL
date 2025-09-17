#pragma once

#include "Node.h"
#include <string>
#include <vector>
#include <utility>

namespace CHTL {

class StyleNode : public Node {
public:
    // e.g., { {"width", "100px"}, {"height", "200px"} }
    std::vector<std::pair<std::string, std::string>> properties;

    std::string ToString() const override;
};

} // namespace CHTL
