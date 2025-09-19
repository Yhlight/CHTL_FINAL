#ifndef NAMESPACE_NODE_H
#define NAMESPACE_NODE_H

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class NamespaceNode : public BaseNode {
public:
    std::string name;
    std::vector<std::unique_ptr<BaseNode>> children;

    void addChild(std::unique_ptr<BaseNode> child);
    void accept(Visitor& visitor) override;
    std::unique_ptr<BaseNode> clone() const override;
};

}

#endif //NAMESPACE_NODE_H
