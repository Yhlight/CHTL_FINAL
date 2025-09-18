#include "CHTL/OperatorNode.h"

namespace CHTL {

String OperatorNode::toString() const {
    return operatorType_;
}

String OperatorNode::toHTML() const {
    return "";
}

bool OperatorNode::isValid() const {
    return !operatorType_.empty();
}

void OperatorNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

OperatorNodePtr OperatorNode::create(const String& type, const SourceLocation& loc) {
    return std::make_shared<OperatorNode>(type, std::vector<String>{}, loc);
}

} // namespace CHTL