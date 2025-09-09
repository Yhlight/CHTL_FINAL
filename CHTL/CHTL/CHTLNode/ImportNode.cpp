#include "ImportNode.hpp"
#include <sstream>

namespace CHTL {

ImportNode::ImportNode(const std::string& importType, size_t line, size_t column)
    : BaseNode(NodeType::IMPORT, line, column), importType_(importType), importPath_(""), importName_("") {}

std::string ImportNode::toHTML() const {
    // 导入节点不直接生成 HTML
    return "";
}

std::string ImportNode::toString() const {
    std::ostringstream oss;
    oss << "ImportNode(type=" << importType_
        << ", line=" << getLine() << ", column=" << getColumn() << ")";
    return oss.str();
}

} // namespace CHTL