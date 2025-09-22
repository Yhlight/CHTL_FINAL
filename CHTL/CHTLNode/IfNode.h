#ifndef IF_NODE_H
#define IF_NODE_H

#include "BaseNode.h"
#include "AttributeNode.h"
#include "../Expression/Expr.h"
#include "Visitor.h"
#include <vector>
#include <memory>

namespace CHTL {

class IfNode : public BaseNode {
public:
    void accept(Visitor& visitor) override;
    std::unique_ptr<BaseNode> clone() const override;

    std::unique_ptr<Expr> condition;
    std::vector<std::unique_ptr<BaseNode>> children; // Can hold elements for conditional rendering
    std::vector<AttributeNode> properties; // Can hold properties for conditional styling
};

} // namespace CHTL

#endif // IF_NODE_H
