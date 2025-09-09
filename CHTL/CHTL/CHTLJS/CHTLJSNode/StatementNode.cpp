#include "StatementNode.hpp"
#include <sstream>

namespace CHTL {

StatementNode::StatementNode(const std::string& content, size_t line, size_t column)
    : CHTLJSBaseNode(NodeType::STATEMENT, line, column), content_(content) {}

std::string StatementNode::toJavaScript() const {
    return content_;
}

std::string StatementNode::toString() const {
    std::ostringstream oss;
    oss << "StatementNode(content=\"" << content_ << "\""
        << ", line=" << getLine() << ", column=" << getColumn() << ")";
    return oss.str();
}

} // namespace CHTL