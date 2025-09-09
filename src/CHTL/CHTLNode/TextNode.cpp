#include "CHTL/CHTLNode/TextNode.h"
#include <cctype>
#include <sstream>

namespace CHTL {

TextNode::TextNode(const std::string& text, size_t line, size_t column)
    : BaseNode(NodeType::TEXT, line, column), text_(text) {
}

void TextNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

std::string TextNode::generate() {
    return escapeHtml(text_);
}

bool TextNode::isEmpty() const {
    return text_.empty();
}

bool TextNode::isWhitespace() const {
    return std::all_of(text_.begin(), text_.end(), [this](char c) {
        return isWhitespaceChar(c);
    });
}

std::string TextNode::toString() const {
    std::ostringstream oss;
    oss << "TextNode(\"" << text_ << "\", " << line_ << ":" << column_ << ")";
    return oss.str();
}

std::string TextNode::escapeHtml(const std::string& text) const {
    std::string result;
    result.reserve(text.length());
    
    for (char c : text) {
        switch (c) {
            case '&':
                result += "&amp;";
                break;
            case '<':
                result += "&lt;";
                break;
            case '>':
                result += "&gt;";
                break;
            case '"':
                result += "&quot;";
                break;
            case '\'':
                result += "&#39;";
                break;
            default:
                result += c;
                break;
        }
    }
    
    return result;
}

bool TextNode::isWhitespaceChar(char c) const {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

} // namespace CHTL