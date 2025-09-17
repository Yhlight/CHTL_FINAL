#ifndef CHTL_ELEMENT_NODE_H
#define CHTL_ELEMENT_NODE_H

#include "BaseNode.h"
#include "AttributeNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class ElementNode : public BaseNode {
public:
    std::string tagName;
    std::vector<std::shared_ptr<AttributeNode>> attributes;
    std::vector<std::shared_ptr<BaseNode>> children;

    std::shared_ptr<BaseNode> clone() const override {
        auto node = std::make_shared<ElementNode>();
        node->tagName = this->tagName;
        for (const auto& attr : this->attributes) {
            node->attributes.push_back(std::dynamic_pointer_cast<AttributeNode>(attr->clone()));
        }
        for (const auto& child : this->children) {
            node->children.push_back(child->clone());
        }
        return node;
    }
};

} // namespace CHTL

#endif // CHTL_ELEMENT_NODE_H
