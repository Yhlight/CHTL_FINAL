#ifndef BASE_NODE_H
#define BASE_NODE_H

#include <memory>
#include "Visitor.h"

namespace CHTL {

class BaseNode {
public:
    virtual ~BaseNode() = default;
    virtual void accept(Visitor& visitor) = 0;
    virtual std::unique_ptr<BaseNode> clone() const = 0;
};

} // namespace CHTL

#endif // BASE_NODE_H
