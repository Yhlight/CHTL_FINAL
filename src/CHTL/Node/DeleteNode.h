#ifndef CHTL_DELETE_NODE_H
#define CHTL_DELETE_NODE_H

#include "BaseNode.h"
#include <string>
#include <vector>

namespace CHTL {

// Represents a 'delete' operation in a specialization body.
// e.g., delete line-height, border;
class DeleteNode : public BaseNode {
public:
    // Stores the names of the properties or templates to be deleted.
    std::vector<std::string> targets;

    std::shared_ptr<BaseNode> clone() const override {
        auto node = std::make_shared<DeleteNode>();
        node->targets = this->targets;
        return node;
    }
};

} // namespace CHTL

#endif // CHTL_DELETE_NODE_H
