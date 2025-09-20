#include "ImportNode.h"
#include "Visitor.h"

namespace CHTL {

void ImportNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<BaseNode> ImportNode::clone() const {
    auto node = std::make_unique<ImportNode>();
    node->category = this->category;
    node->type = this->type;
    node->name = this->name;
    node->path = this->path;
    node->alias = this->alias;
    return node;
}

} // namespace CHTL
