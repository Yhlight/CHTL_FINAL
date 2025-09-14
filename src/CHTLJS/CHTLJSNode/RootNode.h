#ifndef CHTL_JS_ROOT_NODE_H
#define CHTL_JS_ROOT_NODE_H

#include "CHTLJSNode.h"
#include <vector>
#include <memory>

namespace CHTLJS {

class RootNode : public CHTLJSNode {
public:
    RootNode() = default;

    CHTLJSNodeType getType() const override { return CHTLJSNodeType::Root; }

    std::unique_ptr<CHTLJSNode> clone() const override {
        auto new_node = std::make_unique<RootNode>();
        for (const auto& stmt : statements_) {
            new_node->addStatement(stmt->clone());
        }
        return new_node;
    }

    void addStatement(std::unique_ptr<CHTLJSNode> statement) {
        statements_.push_back(std::move(statement));
    }

    const std::vector<std::unique_ptr<CHTLJSNode>>& getStatements() const {
        return statements_;
    }

private:
    std::vector<std::unique_ptr<CHTLJSNode>> statements_;
};

} // namespace CHTLJS

#endif // CHTL_JS_ROOT_NODE_H
