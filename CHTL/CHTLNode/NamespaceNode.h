#ifndef NAMESPACE_NODE_H
#define NAMESPACE_NODE_H

#include "BaseNode.h"
#include "Visitor.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class NamespaceNode : public BaseNode {
public:
    explicit NamespaceNode(const std::string& name) : name(name) {}

    void accept(Visitor& visitor) override;
    std::unique_ptr<BaseNode> clone() const override;

    void addChild(std::unique_ptr<BaseNode> child) {
        children.push_back(std::move(child));
    }

    std::string name;
    std::vector<std::unique_ptr<BaseNode>> children;
};

} // namespace CHTL

#endif // NAMESPACE_NODE_H
