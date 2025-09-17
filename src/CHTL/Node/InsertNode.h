#ifndef CHTL_INSERT_NODE_H
#define CHTL_INSERT_NODE_H

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

enum class InsertPosition {
    AFTER,
    BEFORE,
    REPLACE,
    AT_TOP,
    AT_BOTTOM
};

class InsertNode : public BaseNode {
public:
    InsertPosition position;
    std::string selector;
    std::vector<std::shared_ptr<BaseNode>> body;

    std::shared_ptr<BaseNode> clone() const override {
        auto node = std::make_shared<InsertNode>();
        node->position = this->position;
        node->selector = this->selector;
        for (const auto& child : this->body) {
            node->body.push_back(child->clone());
        }
        return node;
    }
};

} // namespace CHTL

#endif // CHTL_INSERT_NODE_H
