#ifndef CHTL_ELEMENT_NODE_H
#define CHTL_ELEMENT_NODE_H

#include "BaseNode.h"
#include "AttributeNode.h"
#include <vector>
#include <string>
#include <memory>

namespace CHTL {

class ElementNode : public Node {
public:
    explicit ElementNode(const std::string& tagName) : tagName_(tagName) {}

    NodeType getType() const override { return NodeType::Element; }

    std::string tagName_;
    std::vector<std::unique_ptr<AttributeNode>> attributes_;
    std::vector<std::unique_ptr<Node>> children_;
};

} // namespace CHTL

#endif // CHTL_ELEMENT_NODE_H
