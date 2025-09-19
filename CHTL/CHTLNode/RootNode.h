#ifndef ROOT_NODE_H
#define ROOT_NODE_H

#include "BaseNode.h"
#include <vector>
#include <memory>

namespace CHTL {

class RootNode : public BaseNode {
public:
    std::vector<std::unique_ptr<BaseNode>> children;

    void addChild(std::unique_ptr<BaseNode> child);
    void accept(Visitor& visitor) override;
    std::unique_ptr<BaseNode> clone() const override;
};

}

#endif //ROOT_NODE_H
