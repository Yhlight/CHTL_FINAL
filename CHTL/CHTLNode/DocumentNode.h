#ifndef CHTL_DOCUMENTNODE_H
#define CHTL_DOCUMENTNODE_H

#include "BaseNode.h"
#include <vector>
#include <memory>

namespace CHTL {

class DocumentNode : public BaseNode {
public:
    DocumentNode() = default;

    void accept(Visitor& visitor) override;

    std::unique_ptr<BaseNode> clone() const override {
        auto new_node = std::make_unique<DocumentNode>();
        for (const auto& child : children) {
            new_node->addChild(child->clone());
        }
        return new_node;
    }

    void addChild(std::unique_ptr<BaseNode> child) {
        children.push_back(std::move(child));
    }

    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const {
        return children;
    }

private:
    std::vector<std::unique_ptr<BaseNode>> children;
};

} // namespace CHTL

#endif // CHTL_DOCUMENTNODE_H
