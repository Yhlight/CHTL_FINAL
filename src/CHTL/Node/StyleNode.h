#ifndef CHTL_STYLE_NODE_H
#define CHTL_STYLE_NODE_H

#include "BaseNode.h"
#include <vector>
#include <memory>

namespace CHTL {

class StyleNode : public BaseNode {
public:
    std::vector<std::shared_ptr<BaseNode>> children;
};

} // namespace CHTL

#endif // CHTL_STYLE_NODE_H
