#include "CHTL/ImportNode.h"

namespace CHTL {

// 实现基类方法
String ImportNode::toString() const {
    return "[" + importType_ + "] " + importPath_;
}

String ImportNode::toHTML() const {
    return "";
}

bool ImportNode::isValid() const {
    return !importType_.empty() && !importPath_.empty();
}

void ImportNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

// 工厂方法
ImportNodePtr ImportNode::create(const String& type, const String& path, const SourceLocation& loc) {
    return std::make_shared<ImportNode>(type, path, "", loc);
}

} // namespace CHTL