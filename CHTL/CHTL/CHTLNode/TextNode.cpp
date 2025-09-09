#include "TextNode.h"
#include <algorithm>
#include <cctype>
#include <sstream>

namespace CHTL {

TextNode::TextNode(const std::string& content, bool isRaw) 
    : BaseNode(NodeType::TEXT, "text"), content(content), isRaw(isRaw) {
}

void TextNode::append(const std::string& text) {
    content += text;
}

void TextNode::prepend(const std::string& text) {
    content = text + content;
}

void TextNode::trim() {
    // 去除前后空白字符
    content.erase(content.begin(), std::find_if(content.begin(), content.end(), 
        [](unsigned char ch) { return !std::isspace(ch); }));
    content.erase(std::find_if(content.rbegin(), content.rend(), 
        [](unsigned char ch) { return !std::isspace(ch); }).base(), content.end());
}

bool TextNode::isEmpty() const {
    return content.empty();
}

size_t TextNode::length() const {
    return content.length();
}

std::string TextNode::escapeHTML() const {
    return escapeHTMLString(content);
}

std::string TextNode::unescapeHTML() const {
    return unescapeHTMLString(content);
}

std::string TextNode::toHTML() const {
    return toHTML(true);
}

std::string TextNode::toHTML(bool escape) const {
    if (isRaw) {
        return content;
    }
    
    if (escape) {
        return escapeHTMLString(content);
    }
    
    return content;
}

bool TextNode::isValid() const {
    return true; // 文本节点总是有效的
}

std::shared_ptr<BaseNode> TextNode::clone() const {
    auto cloned = std::make_shared<TextNode>(content, isRaw);
    cloned->line = line;
    cloned->column = column;
    cloned->position = position;
    return cloned;
}

std::string TextNode::debugString() const {
    std::ostringstream oss;
    oss << "TextNode: \"" << content << "\" (Line: " << line << ", Column: " << column << ")";
    if (isRaw) {
        oss << " [RAW]";
    }
    return oss.str();
}

std::string TextNode::escapeHTMLString(const std::string& str) {
    std::string result;
    result.reserve(str.length() * 2); // 预分配空间
    
    for (char c : str) {
        switch (c) {
            case '&':  result += "&amp;";  break;
            case '<':  result += "&lt;";   break;
            case '>':  result += "&gt;";   break;
            case '"':  result += "&quot;"; break;
            case '\'': result += "&#39;";  break;
            default:   result += c;        break;
        }
    }
    
    return result;
}

std::string TextNode::unescapeHTMLString(const std::string& str) {
    std::string result = str;
    
    // 替换HTML实体
    size_t pos = 0;
    while ((pos = result.find("&amp;", pos)) != std::string::npos) {
        result.replace(pos, 5, "&");
        pos += 1;
    }
    
    pos = 0;
    while ((pos = result.find("&lt;", pos)) != std::string::npos) {
        result.replace(pos, 4, "<");
        pos += 1;
    }
    
    pos = 0;
    while ((pos = result.find("&gt;", pos)) != std::string::npos) {
        result.replace(pos, 4, ">");
        pos += 1;
    }
    
    pos = 0;
    while ((pos = result.find("&quot;", pos)) != std::string::npos) {
        result.replace(pos, 6, "\"");
        pos += 1;
    }
    
    pos = 0;
    while ((pos = result.find("&#39;", pos)) != std::string::npos) {
        result.replace(pos, 5, "'");
        pos += 1;
    }
    
    return result;
}

} // namespace CHTL