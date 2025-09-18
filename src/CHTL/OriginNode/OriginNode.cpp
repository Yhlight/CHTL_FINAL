#include "CHTL/OriginNode.h"

namespace CHTL {

String OriginNode::toString() const {
    return "[" + originType_ + "] " + originName_;
}

String OriginNode::toHTML() const {
    return content_;
}

bool OriginNode::isValid() const {
    return !originType_.empty();
}

void OriginNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

OriginNodePtr OriginNode::create(const String& type, const String& name, const SourceLocation& loc) {
    return std::make_shared<OriginNode>(type, name, "", loc);
}

} // namespace CHTL