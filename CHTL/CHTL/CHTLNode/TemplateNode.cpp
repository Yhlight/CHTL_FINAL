#include "TemplateNode.hpp"
#include <sstream>

namespace CHTL {

TemplateNode::TemplateNode(const std::string& templateType, size_t line, size_t column)
    : BaseNode(NodeType::TEMPLATE, line, column), templateType_(templateType) {}

std::string TemplateNode::toHTML() const {
    // 模板节点不直接生成 HTML，而是被其他节点引用
    return "";
}

std::string TemplateNode::toString() const {
    std::ostringstream oss;
    oss << "TemplateNode(type=" << templateType_
        << ", line=" << getLine() << ", column=" << getColumn()
        << ", children=" << getChildCount() << ")";
    return oss.str();
}

} // namespace CHTL