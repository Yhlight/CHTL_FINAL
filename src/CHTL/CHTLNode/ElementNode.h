#ifndef CHTL_ELEMENT_NODE_H
#define CHTL_ELEMENT_NODE_H

#include "BaseNode.h"
#include "AttributeNode.h"
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <variant>

namespace CHTL {

enum class BannedNodeType {
    Html,
    Template,
    Custom,
    VarTemplate,
    StyleTemplate,
    ElementTemplate
};

struct PreciseConstraint {
    std::string name;
    // Could also store a NodeType enum if needed
};

struct TypeConstraint {
    BannedNodeType node_type;
};

using Constraint = std::variant<PreciseConstraint, TypeConstraint>;

class ElementNode : public Node {
public:
    explicit ElementNode(const std::string& tagName) : tagName_(tagName) {}

    NodeType getType() const override { return NodeType::Element; }

    std::unique_ptr<Node> clone() const override {
        auto new_node = std::make_unique<ElementNode>(tagName_);
        for (const auto& attr : this->attributes_) {
            new_node->attributes_.push_back(std::unique_ptr<AttributeNode>(static_cast<AttributeNode*>(attr->clone().release())));
        }
        for (const auto& child : this->children_) {
            new_node->children_.push_back(child->clone());
        }
        new_node->computed_styles_ = this->computed_styles_;
        new_node->constraints_ = this->constraints_;
        return new_node;
    }

    std::string tagName_;
    std::vector<std::unique_ptr<AttributeNode>> attributes_;
    std::vector<std::unique_ptr<Node>> children_;
    std::map<std::string, std::string> computed_styles_;
    std::vector<Constraint> constraints_;
};

} // namespace CHTL

#endif // CHTL_ELEMENT_NODE_H
