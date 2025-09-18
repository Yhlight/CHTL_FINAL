#include "StyleNode.h"

namespace CHTL {

void StyleNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
