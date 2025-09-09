#include "ConfigNode.hpp"
#include <sstream>

namespace CHTL {

ConfigNode::ConfigNode(size_t line, size_t column)
    : BaseNode(NodeType::CONFIG, line, column) {}

std::string ConfigNode::toHTML() const {
    // 配置节点不直接生成 HTML
    return "";
}

std::string ConfigNode::toString() const {
    std::ostringstream oss;
    oss << "ConfigNode(line=" << getLine() << ", column=" << getColumn()
        << ", attributes=" << getAttributes().size() << ")";
    return oss.str();
}

} // namespace CHTL