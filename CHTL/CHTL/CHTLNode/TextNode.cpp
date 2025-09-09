#include "TextNode.hpp"
#include <sstream>
#include <cctype>
#include <algorithm>

namespace CHTL {

TextNode::TextNode(const std::string& content, size_t line, size_t column)
    : BaseNode(NodeType::TEXT, line, column) {
    setValue(content);
}

bool TextNode::isEmpty() const {
    return getValue().empty();
}

bool TextNode::isWhitespace() const {
    const std::string& content = getValue();
    return std::all_of(content.begin(), content.end(), [](char c) {
        return std::isspace(c);
    });
}

std::string TextNode::toHTML() const {
    const std::string& content = getValue();
    
    // 如果是空白文本，直接返回
    if (isWhitespace()) {
        return content;
    }
    
    // 转义 HTML 特殊字符
    return escapeHTML(content);
}

std::string TextNode::toString() const {
    std::ostringstream oss;
    oss << "TextNode(content=\"" << getValue() 
        << "\", line=" << getLine() << ", column=" << getColumn() << ")";
    return oss.str();
}

std::string TextNode::escapeHTML(const std::string& text) const {
    std::string result;
    result.reserve(text.length() * 2);
    
    for (char c : text) {
        switch (c) {
            case '&':  result += "&amp;"; break;
            case '<':  result += "&lt;"; break;
            case '>':  result += "&gt;"; break;
            case '"':  result += "&quot;"; break;
            case '\'': result += "&#39;"; break;
            default:   result += c; break;
        }
    }
    
    return result;
}

} // namespace CHTL