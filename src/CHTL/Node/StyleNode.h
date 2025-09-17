#ifndef CHTL_STYLE_NODE_H
#define CHTL_STYLE_NODE_H

#include "BaseNode.h"
#include <vector>
#include <memory>

namespace CHTL {

class StyleNode : public BaseNode {
public:
    std::vector<std::shared_ptr<BaseNode>> children;

    std::shared_ptr<BaseNode> clone() const override {
        auto node = std::make_shared<StyleNode>();
        for (const auto& child : this->children) {
            node->children.push_back(child->clone());
        }
        return node;
    }
};

} // namespace CHTL

#endif // CHTL_STYLE_NODE_H
