#ifndef CHTL_ELEMENT_NODE_H
#define CHTL_ELEMENT_NODE_H

#include "BaseNode.h"
#include "AttributeNode.h"
#include "ConstraintNode.h"
#include <vector>
#include <string>
#include <memory>

namespace CHTL {

class ElementNode : public Node {
public:
    explicit ElementNode(const std::string& tagName) : tagName_(tagName) {}

    NodeType getType() const override { return NodeType::Element; }
    const char* getTypeName() const override { return "Element"; }

    std::unique_ptr<Node> clone() const override {
        auto new_node = std::make_unique<ElementNode>(tagName_);
        for (const auto& attr : this->attributes_) {
            new_node->attributes_.push_back(attr->clone());
        }
        for (const auto& child : this->children_) {
            new_node->children_.push_back(child->clone());
        }
        new_node->constraints_ = this->constraints_;
        return new_node;
    }

    std::string tagName_;
    std::vector<std::unique_ptr<AttributeNode>> attributes_;
    std::vector<std::unique_ptr<Node>> children_;
    std::vector<Constraint> constraints_;
};

} // namespace CHTL

#endif // CHTL_ELEMENT_NODE_H
