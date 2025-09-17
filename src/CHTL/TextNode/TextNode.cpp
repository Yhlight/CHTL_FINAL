#include "CHTL/TextNode.h"
#include <algorithm>
#include <cctype>

namespace CHTL {

// 检查是否只包含空白字符
bool TextNode::isWhitespace() const {
    return std::all_of(content_.begin(), content_.end(), [](char c) {
        return std::isspace(c);
    });
}

// 转义HTML特殊字符
String TextNode::escapeHTML() const {
    String result = content_;
    
    // 替换HTML特殊字符
    std::unordered_map<char, String> replacements = {
        {'&', "&amp;"},
        {'<', "&lt;"},
        {'>', "&gt;"},
        {'"', "&quot;"},
        {'\'', "&#39;"}
    };
    
    for (const auto& replacement : replacements) {
        size_t pos = 0;
        while ((pos = result.find(replacement.first, pos)) != String::npos) {
            result.replace(pos, 1, replacement.second);
            pos += replacement.second.length();
        }
    }
    
    return result;
}

// 实现基类方法
String TextNode::toString() const {
    return content_;
}

String TextNode::toHTML() const {
    if (isRawText_) {
        return content_;
    }
    return escapeHTML();
}

bool TextNode::isValid() const {
    // 文本节点总是有效的
    return true;
}

void TextNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

// 工厂方法
TextNodePtr TextNode::create(const String& content, const SourceLocation& loc) {
    return std::make_shared<TextNode>(content, loc);
}

TextNodePtr TextNode::createRaw(const String& content, const SourceLocation& loc) {
    auto node = create(content, loc);
    node->setRawText(true);
    return node;
}

TextNodePtr TextNode::createEmpty(const SourceLocation& loc) {
    return create("", loc);
}

TextNodePtr TextNode::createWhitespace(const SourceLocation& loc) {
    return create(" ", loc);
}

} // namespace CHTL