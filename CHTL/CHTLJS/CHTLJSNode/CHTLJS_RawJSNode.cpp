#include "CHTLJS_RawJSNode.h"
#include "CHTLJS_Visitor.h"

namespace CHTLJS {

void CHTLJS_RawJSNode::accept(CHTLJS_Visitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTLJS
