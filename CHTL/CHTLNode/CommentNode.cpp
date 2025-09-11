#include "CommentNode.hpp"
#include "AstVisitor.hpp"

namespace CHTL {

void CommentNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
