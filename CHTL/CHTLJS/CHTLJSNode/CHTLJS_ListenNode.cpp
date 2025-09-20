#include "CHTLJS_ListenNode.h"
#include "CHTLJS_Visitor.h"

namespace CHTLJS {

void CHTLJS_ListenNode::accept(CHTLJS_Visitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<CHTLJS_BaseNode> CHTLJS_ListenNode::clone() const {
    auto new_node = std::make_unique<CHTLJS_ListenNode>(target->clone());
    new_node->handlers = this->handlers;
    return new_node;
}

} // namespace CHTLJS
