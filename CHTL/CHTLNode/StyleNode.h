#pragma once

#include "Node.h"
#include <string>
#include <vector>
#include <utility>
#include <memory>

namespace CHTL {

class StyleNode : public Node {
public:
    // A style block can contain direct properties and template usages
    std::vector<std::unique_ptr<Node>> items;

    std::string ToString() const override;
};

} // namespace CHTL
