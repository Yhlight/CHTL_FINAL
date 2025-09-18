#include "DocumentNode.h"
#include "Visitor.h"

namespace CHTL {

void DocumentNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
