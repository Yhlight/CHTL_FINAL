#include "TemplateNode.h"
#include "NodeVisitor.h"

namespace CHTL {

void TemplateNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
