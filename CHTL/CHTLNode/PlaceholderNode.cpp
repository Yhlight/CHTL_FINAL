#include "PlaceholderNode.h"
#include "Visitor.h"

namespace CHTL {

PlaceholderNode::PlaceholderNode(const std::string& placeholder) : placeholder(placeholder) {}

void PlaceholderNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<BaseNode> PlaceholderNode::clone() const {
    return std::make_unique<PlaceholderNode>(placeholder);
}

} // namespace CHTL
