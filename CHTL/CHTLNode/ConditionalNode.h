#ifndef CONDITIONAL_NODE_H
#define CONDITIONAL_NODE_H

#include "BaseNode.h"
#include "AttributeNode.h"
#include <vector>
#include <memory>

namespace CHTL {

class ConditionalNode : public BaseNode {
public:
    ConditionalNode();

    void accept(Visitor& visitor) override;
    std::unique_ptr<BaseNode> clone() const override;

    std::vector<AttributeNode> properties;
    std::unique_ptr<BaseNode> else_branch;
};

} // namespace CHTL

#endif // CONDITIONAL_NODE_H
