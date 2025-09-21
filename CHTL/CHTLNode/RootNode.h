#ifndef ROOT_NODE_H
#define ROOT_NODE_H

#include "BaseNode.h"
#include <vector>
#include <memory>

namespace CHTL {

class RootNode : public BaseNode {
public:
    void accept(Visitor& visitor) override;
    std::unique_ptr<BaseNode> clone() const override;

    void addChild(std::unique_ptr<BaseNode> child);

    std::vector<std::unique_ptr<BaseNode>> children;
};

} // namespace CHTL

#endif // ROOT_NODE_H
