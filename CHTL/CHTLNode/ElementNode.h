#pragma once

#include "Node.h"
#include <string>
#include <vector>
#include <memory>
#include <utility> // for std::pair

namespace CHTL {

class ElementNode : public Node {
public:
    std::string tagName;
    std::vector<std::pair<std::string, std::string>> attributes;
    std::vector<std::unique_ptr<Node>> children;

    std::string ToString() const override;
};

} // namespace CHTL
