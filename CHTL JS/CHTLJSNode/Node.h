#pragma once

#include <string>
#include <memory>
#include <vector>

namespace CHTL_JS {

class Node {
public:
    virtual ~Node() = default;
    virtual std::string ToString() const = 0;
};

using NodePtr = std::shared_ptr<Node>;
using NodeList = std::vector<NodePtr>;

} // namespace CHTL_JS
