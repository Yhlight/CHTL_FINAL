#include "OriginNode.hpp"
#include <sstream>

namespace CHTL {

OriginNode::OriginNode(const std::string& originType, size_t line, size_t column)
    : BaseNode(NodeType::ORIGIN, line, column), originType_(originType) {}

std::string OriginNode::toHTML() const {
    // 原始嵌入节点直接输出内容
    return getValue();
}

std::string OriginNode::toString() const {
    std::ostringstream oss;
    oss << "OriginNode(type=" << originType_
        << ", line=" << getLine() << ", column=" << getColumn()
        << ", content=\"" << getValue() << "\")";
    return oss.str();
}

} // namespace CHTL