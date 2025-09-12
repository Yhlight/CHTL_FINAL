#ifndef CHTL_NAMESPACENODE_H
#define CHTL_NAMESPACENODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

// Represents a [Namespace] block
class NamespaceNode : public BaseNode {
public:
    explicit NamespaceNode(std::string name) : name(std::move(name)) {}

    void accept(NodeVisitor& visitor) override;

private:
    std::string name;
};

} // namespace CHTL

#endif // CHTL_NAMESPACENODE_H
