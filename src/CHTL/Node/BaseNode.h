#ifndef CHTL_BASE_NODE_H
#define CHTL_BASE_NODE_H

#include <memory>
#include <string>
#include <vector>

namespace CHTL {

class BaseNode {
public:
    std::weak_ptr<BaseNode> parent;

    virtual ~BaseNode() = default;
    virtual std::shared_ptr<BaseNode> clone() const = 0;
};

} // namespace CHTL

#endif // CHTL_BASE_NODE_H
