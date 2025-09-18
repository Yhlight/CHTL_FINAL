#include "CHTL/TemplateNode.h"

namespace CHTL {

// 参数管理
void TemplateNode::setParameter(const String& name, const String& value) {
    parameters_[name] = value;
}

String TemplateNode::getParameter(const String& name) const {
    auto it = parameters_.find(name);
    return it != parameters_.end() ? it->second : "";
}

bool TemplateNode::hasParameter(const String& name) const {
    return parameters_.find(name) != parameters_.end();
}

void TemplateNode::removeParameter(const String& name) {
    parameters_.erase(name);
}

void TemplateNode::clearParameters() {
    parameters_.clear();
}

// 实现基类方法
String TemplateNode::toString() const {
    return "[" + templateType_ + "] " + templateName_;
}

String TemplateNode::toHTML() const {
    return "";
}

bool TemplateNode::isValid() const {
    return !templateType_.empty() && !templateName_.empty();
}

void TemplateNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

// 工厂方法
TemplateNodePtr TemplateNode::create(const String& type, const String& name, const SourceLocation& loc) {
    return std::make_shared<TemplateNode>(type, name, loc);
}

} // namespace CHTL