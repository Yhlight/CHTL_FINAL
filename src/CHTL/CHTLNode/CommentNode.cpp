#include "CommentNode.h"
#include "NodeVisitor.h"

namespace CHTL {

void CommentNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
