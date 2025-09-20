#include "TextNode.h"
#include "Visitor.h"

namespace CHTL {

void TextNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<BaseNode> TextNode::clone() const {
    return std::make_unique<TextNode>(this->text);
}

} // namespace CHTL
