#ifndef CHTL_ROOT_NODE_H
#define CHTL_ROOT_NODE_H

#include "BaseNode.h"
#include <vector>
#include <memory>

namespace CHTL {

class RootNode : public BaseNode {
public:
    std::vector<std::shared_ptr<BaseNode>> children;
};

} // namespace CHTL

#endif // CHTL_ROOT_NODE_H
