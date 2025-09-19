#include "ImportNode.h"

namespace CHTL {

void ImportNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<BaseNode> ImportNode::clone() const {
    auto node = std::make_unique<ImportNode>();
    node->info = this->info;
    return node;
}

}
