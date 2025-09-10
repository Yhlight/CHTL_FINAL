#include "TextNode.h"
#include <algorithm>
#include <sstream>
#include <cctype>

namespace CHTL {

TextNode::TextNode(const std::string& text, bool isLiteral)
    : BaseNode(NodeType::TEXT, "text", text), isLiteral(isLiteral), needsEscape(true) {
}

std::string TextNode::getTrimmedText() const {
    std::string trimmed = value;
    
    // 去除前导空白
    trimmed.erase(trimmed.begin(), std::find_if(trimmed.begin(), trimmed.end(),
        [](unsigned char ch) { return !std::isspace(ch); }));
    
    // 去除尾随空白
    trimmed.erase(std::find_if(trimmed.rbegin(), trimmed.rend(),
        [](unsigned char ch) { return !std::isspace(ch); }).base(), trimmed.end());
    
    return trimmed;
}

std::string TextNode::getEscapedText() const {
    if (!needsEscape) {
        return value;
    }
    return escapeHTML(value);
}

bool TextNode::isEmpty() const {
    return value.empty();
}

bool TextNode::isWhitespace() const {
    return std::all_of(value.begin(), value.end(), [this](char c) {
        return isWhitespaceChar(c);
    });
}

bool TextNode::isValid() const {
    return true; // 文本节点总是有效的
}

std::string TextNode::toHTML() const {
    if (isWhitespace()) {
        return value; // 保留空白字符
    }
    
    if (needsEscape) {
        return escapeHTML(value);
    }
    
    return value;
}

std::string TextNode::getDebugInfo() const {
    std::ostringstream oss;
    oss << BaseNode::getDebugInfo();
    oss << "Text: \"" << value << "\"\n";
    oss << "IsLiteral: " << (isLiteral ? "true" : "false") << "\n";
    oss << "NeedsEscape: " << (needsEscape ? "true" : "false") << "\n";
    return oss.str();
}

std::shared_ptr<BaseNode> TextNode::clone() const {
    auto cloned = std::make_shared<TextNode>(value, isLiteral);
    cloned->needsEscape = needsEscape;
    cloned->line = line;
    cloned->column = column;
    cloned->position = position;
    return cloned;
}

bool TextNode::operator==(const TextNode& other) const {
    return BaseNode::operator==(other) &&
           isLiteral == other.isLiteral &&
           needsEscape == other.needsEscape;
}

bool TextNode::operator!=(const TextNode& other) const {
    return !(*this == other);
}

std::string TextNode::escapeHTML(const std::string& text) const {
    std::string escaped;
    escaped.reserve(text.length() * 1.2); // 预分配空间
    
    for (char c : text) {
        switch (c) {
            case '&':  escaped += "&amp;"; break;
            case '<':  escaped += "&lt;"; break;
            case '>':  escaped += "&gt;"; break;
            case '"':  escaped += "&quot;"; break;
            case '\'': escaped += "&#39;"; break;
            default:   escaped += c; break;
        }
    }
    
    return escaped;
}

std::string TextNode::unescapeHTML(const std::string& text) const {
    std::string unescaped = text;
    
    // 简单的HTML实体解码
    std::map<std::string, char> entities = {
        {"&amp;", '&'},
        {"&lt;", '<'},
        {"&gt;", '>'},
        {"&quot;", '"'},
        {"&#39;", '\''},
        {"&apos;", '\''}
    };
    
    for (const auto& entity : entities) {
        size_t pos = 0;
        while ((pos = unescaped.find(entity.first, pos)) != std::string::npos) {
            unescaped.replace(pos, entity.first.length(), 1, entity.second);
            pos += 1;
        }
    }
    
    return unescaped;
}

bool TextNode::isWhitespaceChar(char c) const {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}

} // namespace CHTL