#include "CHTL/ScriptNode.h"

namespace CHTL {

// 实现基类方法
String ScriptNode::toString() const {
    if (isGlobal_) {
        return "<script>" + content_ + "</script>";
    } else {
        return "script { " + content_ + " }";
    }
}

String ScriptNode::toHTML() const {
    if (isGlobal_) {
        return "<script>\n" + content_ + "\n</script>";
    } else {
        return content_;
    }
}

bool ScriptNode::isValid() const {
    return true;
}

void ScriptNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

// 工厂方法
ScriptNodePtr ScriptNode::create(const SourceLocation& loc) {
    return std::make_shared<ScriptNode>(loc);
}

ScriptNodePtr ScriptNode::createGlobal(const SourceLocation& loc) {
    auto node = create(loc);
    node->setGlobal(true);
    return node;
}

ScriptNodePtr ScriptNode::createLocal(const SourceLocation& loc) {
    auto node = create(loc);
    node->setLocal(true);
    return node;
}

ScriptNodePtr ScriptNode::createWithContent(const String& content, const SourceLocation& loc) {
    auto node = create(loc);
    node->setContent(content);
    return node;
}

} // namespace CHTL