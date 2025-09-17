#include "CHTL/CustomNode.h"

namespace CHTL {

// 参数管理
void CustomNode::setParameter(const String& name, const String& value) {
    parameters_[name] = value;
}

String CustomNode::getParameter(const String& name) const {
    auto it = parameters_.find(name);
    return it != parameters_.end() ? it->second : "";
}

bool CustomNode::hasParameter(const String& name) const {
    return parameters_.find(name) != parameters_.end();
}

void CustomNode::removeParameter(const String& name) {
    parameters_.erase(name);
}

void CustomNode::clearParameters() {
    parameters_.clear();
}

// 实现基类方法
String CustomNode::toString() const {
    return "[" + customType_ + "] " + customName_;
}

String CustomNode::toHTML() const {
    return "";
}

bool CustomNode::isValid() const {
    return !customType_.empty() && !customName_.empty();
}

void CustomNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

// 工厂方法
CustomNodePtr CustomNode::create(const String& type, const String& name, const SourceLocation& loc) {
    return std::make_shared<CustomNode>(type, name, loc);
}

} // namespace CHTL