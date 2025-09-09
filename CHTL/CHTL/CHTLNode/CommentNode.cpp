#include "CommentNode.hpp"
#include <sstream>
#include <cctype>
#include <algorithm>

namespace CHTL {

CommentNode::CommentNode(const std::string& content, CommentType type, size_t line, size_t column)
    : BaseNode(NodeType::COMMENT, line, column), commentType_(type) {
    setValue(content);
}

bool CommentNode::isEmpty() const {
    return getValue().empty();
}

bool CommentNode::isWhitespace() const {
    const std::string& content = getValue();
    return std::all_of(content.begin(), content.end(), [](char c) {
        return std::isspace(c);
    });
}

std::string CommentNode::toHTML() const {
    const std::string& content = getValue();
    
    // 根据注释类型生成不同的HTML输出
    switch (commentType_) {
        case CommentType::HTML_COMMENT:
            return "<!-- " + content + " -->";
            
        case CommentType::CSS_COMMENT:
        case CommentType::MULTI_LINE:
            return "/* " + content + " */";
            
        case CommentType::SINGLE_LINE:
        default:
            // 单行注释在HTML中不显示，但可以用于调试
            return "";
    }
}

std::string CommentNode::toString() const {
    std::ostringstream oss;
    std::string typeStr;
    switch (commentType_) {
        case CommentType::SINGLE_LINE: typeStr = "SINGLE_LINE"; break;
        case CommentType::MULTI_LINE: typeStr = "MULTI_LINE"; break;
        case CommentType::HTML_COMMENT: typeStr = "HTML_COMMENT"; break;
        case CommentType::CSS_COMMENT: typeStr = "CSS_COMMENT"; break;
    }
    
    oss << "CommentNode(type=" << typeStr 
        << ", content=\"" << getValue() 
        << "\", line=" << getLine() << ", column=" << getColumn() << ")";
    return oss.str();
}

std::string CommentNode::escapeHTML(const std::string& text) const {
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