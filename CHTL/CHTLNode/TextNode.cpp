#include "TextNode.hpp"
#include "AstVisitor.hpp"

namespace CHTL {

void TextNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
