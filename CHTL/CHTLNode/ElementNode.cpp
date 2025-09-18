#include "ElementNode.h"
#include "Visitor.h"

namespace CHTL {

void ElementNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<BaseNode> ElementNode::clone() const {
    auto new_node = std::make_unique<ElementNode>(this->tagName);
    new_node->attributes = this->attributes; // Simple copy is fine now
    for (const auto& child : this->children) {
        new_node->addChild(child->clone());
    }
    return new_node;
}

} // namespace CHTL
