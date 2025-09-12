#include "CHTL/CHTLNode/TextNode.h"
#include <sstream>
#include <algorithm>
#include <cctype>

namespace CHTL {

TextNode::TextNode(const std::string& text)
    : BaseNode(NodeType::TEXT)
    , m_raw(false)
    , m_generatorComment(false)
{
    setText(text);
}

TextNode::~TextNode() {
}

void TextNode::setText(const std::string& text) {
    m_value = text;
}

const std::string& TextNode::getText() const {
    return m_value;
}

void TextNode::appendText(const std::string& text) {
    m_value += text;
}

bool TextNode::isEmpty() const {
    return m_value.empty();
}

bool TextNode::isWhitespace() const {
    return std::all_of(m_value.begin(), m_value.end(), ::isspace);
}

void TextNode::trim() {
    // 去除前导空白字符
    m_value.erase(m_value.begin(), std::find_if(m_value.begin(), m_value.end(), 
        [](unsigned char ch) { return !std::isspace(ch); }));
    
    // 去除尾随空白字符
    m_value.erase(std::find_if(m_value.rbegin(), m_value.rend(), 
        [](unsigned char ch) { return !std::isspace(ch); }).base(), m_value.end());
}

void TextNode::setRaw(bool raw) {
    m_raw = raw;
}

bool TextNode::isRaw() const {
    return m_raw;
}

void TextNode::setGeneratorComment(bool generatorComment) {
    m_generatorComment = generatorComment;
}

bool TextNode::isGeneratorComment() const {
    return m_generatorComment;
}

std::shared_ptr<BaseNode> TextNode::clone() const {
    auto cloned = std::make_shared<TextNode>(m_value);
    cloned->m_raw = m_raw;
    cloned->m_generatorComment = m_generatorComment;
    cloned->m_line = m_line;
    cloned->m_column = m_column;
    
    // 克隆子节点
    for (const auto& child : m_children) {
        if (child) {
            cloned->addChild(child->clone());
        }
    }
    
    return cloned;
}

std::string TextNode::toString() const {
    std::ostringstream oss;
    oss << "TextNode(text=\"" << m_value << "\""
        << ", raw=" << (m_raw ? "true" : "false")
        << ", generatorComment=" << (m_generatorComment ? "true" : "false")
        << ", line=" << m_line << ", column=" << m_column << ")";
    return oss.str();
}

// void TextNode::accept(NodeVisitor& visitor) {
//     visitor.visit(*this);
// }

std::string TextNode::escapeHtml(const std::string& text) {
    std::string result;
    result.reserve(text.length() * 2);
    
    for (char c : text) {
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

std::string TextNode::unescapeHtml(const std::string& text) {
    std::string result = text;
    
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