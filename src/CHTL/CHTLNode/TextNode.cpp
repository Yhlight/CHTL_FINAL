#include "TextNode.h"
#include <algorithm>
#include <cctype>

namespace CHTL {

TextNode::TextNode(const std::string& text)
    : BaseNode(NodeType::TEXT, "text") {
    setText(text);
}

void TextNode::setText(const std::string& text) {
    content_ = text;
}

const std::string& TextNode::getText() const {
    return content_;
}

void TextNode::appendText(const std::string& text) {
    content_ += text;
}

void TextNode::prependText(const std::string& text) {
    content_ = text + content_;
}

void TextNode::trimText() {
    // 去除首尾空白字符
    content_.erase(content_.begin(), std::find_if(content_.begin(), content_.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    
    content_.erase(std::find_if(content_.rbegin(), content_.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), content_.end());
}

std::string TextNode::toHTML() const {
    return escapeHTML(processText());
}

std::string TextNode::toString() const {
    std::ostringstream oss;
    oss << "TextNode(\"" << content_ << "\", " << line_ << ":" << column_ << ")";
    return oss.str();
}

bool TextNode::isValid() const {
    // 文本节点总是有效的，即使是空文本
    return true;
}

std::shared_ptr<TextNode> TextNode::create(const std::string& text) {
    return std::make_shared<TextNode>(text);
}

std::string TextNode::processText() const {
    std::string result = content_;
    
    // 处理特殊字符
    // 这里可以添加更多的文本处理逻辑
    // 比如处理换行符、制表符等
    
    return result;
}

bool TextNode::isWhitespaceOnly() const {
    return std::all_of(content_.begin(), content_.end(), [](unsigned char ch) {
        return std::isspace(ch);
    });
}

} // namespace CHTL