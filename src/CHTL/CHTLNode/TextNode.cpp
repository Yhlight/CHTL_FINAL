#include "TextNode.h"
#include <algorithm>
#include <cctype>
#include <sstream>

namespace CHTL {

TextNode::TextNode(const std::string& content) 
    : BaseNode(NodeType::TEXT), m_content(content), m_isLiteral(false) {}

void TextNode::appendContent(const std::string& content) {
    m_content += content;
}

bool TextNode::isEmpty() const {
    return m_content.empty();
}

bool TextNode::isWhitespace() const {
    return std::all_of(m_content.begin(), m_content.end(), isWhitespaceChar);
}

void TextNode::trim() {
    // 去除首尾空白字符
    size_t start = 0;
    while (start < m_content.length() && isWhitespaceChar(m_content[start])) {
        start++;
    }
    
    size_t end = m_content.length();
    while (end > start && isWhitespaceChar(m_content[end - 1])) {
        end--;
    }
    
    m_content = m_content.substr(start, end - start);
}

std::string TextNode::getTrimmedContent() const {
    std::string result = m_content;
    
    // 去除首尾空白字符
    size_t start = 0;
    while (start < result.length() && isWhitespaceChar(result[start])) {
        start++;
    }
    
    size_t end = result.length();
    while (end > start && isWhitespaceChar(result[end - 1])) {
        end--;
    }
    
    return result.substr(start, end - start);
}

bool TextNode::needsQuotes() const {
    if (m_isLiteral) {
        // 检查是否包含需要引号的字符
        return std::any_of(m_content.begin(), m_content.end(), [](char c) {
            return c == ' ' || c == '\t' || c == '\n' || c == '\r' || 
                   c == '{' || c == '}' || c == ';' || c == ':' || c == '=';
        });
    }
    return false;
}

std::string TextNode::getQuotedContent() const {
    if (needsQuotes()) {
        return "\"" + escapeQuotes(m_content) + "\"";
    }
    return m_content;
}

std::string TextNode::getEscapedContent() const {
    return escapeHTML(m_content);
}

std::unique_ptr<BaseNode> TextNode::clone() const {
    auto cloned = std::make_unique<TextNode>(m_content);
    cloned->m_isLiteral = m_isLiteral;
    cloned->m_line = m_line;
    cloned->m_column = m_column;
    
    if (m_token) {
        cloned->m_token = std::make_unique<Token>(*m_token);
    }
    
    return cloned;
}

void TextNode::accept(NodeVisitor* visitor) {
    if (visitor) {
        visitor->visitText(this);
    }
}

std::string TextNode::toString() const {
    std::stringstream ss;
    ss << "TextNode(\"" << m_content << "\"";
    if (m_isLiteral) {
        ss << ", literal";
    }
    ss << ")";
    return ss.str();
}

std::string TextNode::toHTML() const {
    return escapeHTML(m_content);
}

bool TextNode::isWhitespaceChar(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool TextNode::isHTMLSpecialChar(char c) {
    return c == '&' || c == '<' || c == '>' || c == '"' || c == '\'';
}

std::string TextNode::escapeHTML(const std::string& text) {
    std::string result = text;
    
    // 替换HTML特殊字符
    size_t pos = 0;
    while ((pos = result.find("&", pos)) != std::string::npos) {
        result.replace(pos, 1, "&amp;");
        pos += 5;
    }
    
    pos = 0;
    while ((pos = result.find("<", pos)) != std::string::npos) {
        result.replace(pos, 1, "&lt;");
        pos += 4;
    }
    
    pos = 0;
    while ((pos = result.find(">", pos)) != std::string::npos) {
        result.replace(pos, 1, "&gt;");
        pos += 4;
    }
    
    pos = 0;
    while ((pos = result.find("\"", pos)) != std::string::npos) {
        result.replace(pos, 1, "&quot;");
        pos += 6;
    }
    
    pos = 0;
    while ((pos = result.find("'", pos)) != std::string::npos) {
        result.replace(pos, 1, "&#39;");
        pos += 5;
    }
    
    return result;
}

std::string TextNode::escapeQuotes(const std::string& text, char quote) {
    std::string result = text;
    
    if (quote == '"') {
        size_t pos = 0;
        while ((pos = result.find("\"", pos)) != std::string::npos) {
            result.replace(pos, 1, "\\\"");
            pos += 2;
        }
    } else if (quote == '\'') {
        size_t pos = 0;
        while ((pos = result.find("'", pos)) != std::string::npos) {
            result.replace(pos, 1, "\\'");
            pos += 2;
        }
    }
    
    return result;
}

} // namespace CHTL