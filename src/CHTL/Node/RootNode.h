#ifndef CHTL_ROOT_NODE_H
#define CHTL_ROOT_NODE_H

#include "BaseNode.h"
#include <vector>
#include <memory>

namespace CHTL {

class RootNode : public BaseNode {
public:
    std::vector<std::shared_ptr<BaseNode>> children;

    std::shared_ptr<BaseNode> clone() const override {
        auto node = std::make_shared<RootNode>();
        for (const auto& child : this->children) {
            node->children.push_back(child->clone());
        }
        return node;
    }
};

} // namespace CHTL

#endif // CHTL_ROOT_NODE_H
