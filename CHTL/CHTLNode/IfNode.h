#ifndef IF_NODE_H
#define IF_NODE_H

#include "BaseNode.h"
#include "../Expression/Expr.h"
#include <vector>
#include <memory>

namespace CHTL {

class IfNode : public BaseNode {
public:
    std::unique_ptr<Expr> condition;
    std::vector<std::unique_ptr<BaseNode>> then_branch;

    // For 'else if' chains. This will be another IfNode.
    std::unique_ptr<BaseNode> else_if_branch;

    // For a final 'else' block.
    std::vector<std::unique_ptr<BaseNode>> else_branch;

    IfNode() = default;

    void accept(Visitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<BaseNode> clone() const override;
};

} // namespace CHTL

#endif // IF_NODE_H
