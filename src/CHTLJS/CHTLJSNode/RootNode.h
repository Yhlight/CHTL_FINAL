#ifndef CHTL_JS_ROOT_NODE_H
#define CHTL_JS_ROOT_NODE_H

#include "CHTLJSNode.h"
#include <vector>
#include <memory>

namespace CHTLJS {

class RootNode : public CHTLJSNode {
public:
    RootNode() : CHTLJSNode(CHTLJSNodeType::Root) {}

    void addChild(std::unique_ptr<CHTLJSNode> child) {
        children_.push_back(std::move(child));
    }

    const std::vector<std::unique_ptr<CHTLJSNode>>& getChildren() const {
        return children_;
    }

    std::unique_ptr<CHTLJSNode> clone() const override {
        auto new_node = std::make_unique<RootNode>();
        for (const auto& child : children_) {
            new_node->addChild(child->clone());
        }
        return new_node;
    }

private:
    std::vector<std::unique_ptr<CHTLJSNode>> children_;
};

} // namespace CHTLJS

#endif // CHTL_JS_ROOT_NODE_H
