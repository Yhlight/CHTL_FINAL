#ifndef CHTL_NAMESPACENODE_H
#define CHTL_NAMESPACENODE_H

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class NamespaceNode : public BaseNode {
public:
    std::string name;
    std::vector<std::unique_ptr<BaseNode>> children;

    explicit NamespaceNode(const std::string& name);

    void accept(Visitor& visitor) override;
    std::unique_ptr<BaseNode> clone() const override;
};

} // namespace CHTL

#endif // CHTL_NAMESPACENODE_H
