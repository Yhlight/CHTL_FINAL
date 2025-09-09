#include "TextNode.h"
#include <sstream>

namespace CHTL {

TextNode::TextNode(const std::string& content)
    : BaseNode(NodeType::TEXT, content) {
}

std::string TextNode::toHTML() const {
    return escapeHTML(getContent());
}

std::string TextNode::toCSS() const {
    return ""; // 文本节点不生成CSS
}

std::string TextNode::toJS() const {
    return ""; // 文本节点不生成JS
}

std::shared_ptr<BaseNode> TextNode::clone() const {
    return std::make_shared<TextNode>(getContent());
}

std::string TextNode::escapeHTML(const std::string& text) const {
    std::ostringstream oss;
    for (char c : text) {
        switch (c) {
            case '<': oss << "&lt;"; break;
            case '>': oss << "&gt;"; break;
            case '&': oss << "&amp;"; break;
            case '"': oss << "&quot;"; break;
            case '\'': oss << "&#39;"; break;
            default: oss << c; break;
        }
    }
    return oss.str();
}

} // namespace CHTL