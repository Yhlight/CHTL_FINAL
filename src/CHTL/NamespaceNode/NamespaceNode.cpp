#include "CHTL/NamespaceNode.h"

namespace CHTL {

// 实现基类方法
String NamespaceNode::toString() const {
    return "[" + namespaceName_ + "]";
}

String NamespaceNode::toHTML() const {
    return "";
}

bool NamespaceNode::isValid() const {
    return !namespaceName_.empty();
}

void NamespaceNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

// 工厂方法
NamespaceNodePtr NamespaceNode::create(const String& name, const SourceLocation& loc) {
    return std::make_shared<NamespaceNode>(name, loc);
}

} // namespace CHTL