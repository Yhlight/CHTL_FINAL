#include "CustomNode.hpp"
#include <sstream>

namespace CHTL {

CustomNode::CustomNode(const std::string& customType, size_t line, size_t column)
    : BaseNode(NodeType::CUSTOM, line, column), customType_(customType) {}

std::string CustomNode::toHTML() const {
    // 自定义节点不直接生成 HTML，而是被其他节点引用
    return "";
}

std::string CustomNode::toString() const {
    std::ostringstream oss;
    oss << "CustomNode(type=" << customType_
        << ", line=" << getLine() << ", column=" << getColumn()
        << ", children=" << getChildCount() << ")";
    return oss.str();
}

} // namespace CHTL