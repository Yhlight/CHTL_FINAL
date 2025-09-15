#include "ElementNode.h"
#include <sstream>

namespace CHTL {

// ElementNode implementation
ElementNode::ElementNode(const std::string& tagName)
    : CHTLBaseNode(NodeType::ELEMENT), tag_name_(tagName), 
      self_closing_(false), has_text_content_(false) {
}

ElementNode::~ElementNode() {
}

void ElementNode::setTagName(const std::string& tagName) {
    tag_name_ = tagName;
}

const std::string& ElementNode::getTagName() const {
    return tag_name_;
}

void ElementNode::setTextContent(const std::string& text) {
    text_content_ = text;
    has_text_content_ = true;
}

const std::string& ElementNode::getTextContent() const {
    return text_content_;
}

bool ElementNode::hasTextContent() const {
    return has_text_content_;
}

void ElementNode::setSelfClosing(bool selfClosing) {
    self_closing_ = selfClosing;
}

bool ElementNode::isSelfClosing() const {
    return self_closing_;
}

std::string ElementNode::generateCode() const {
    std::stringstream ss;
    
    // 开始标签
    ss << "<" << tag_name_;
    
    // 属性
    for (const auto& attr : getAttributes()) {
        ss << " " << attr.first;
        if (!attr.second.empty()) {
            ss << "=\"" << attr.second << "\"";
        }
    }
    
    if (self_closing_) {
        ss << " />";
        return ss.str();
    }
    
    ss << ">";
    
    // 文本内容
    if (has_text_content_) {
        ss << text_content_;
    }
    
    // 子节点
    for (const auto& child : getChildren()) {
        ss << child->generateCode();
    }
    
    // 结束标签
    ss << "</" << tag_name_ << ">";
    
    return ss.str();
}

std::string ElementNode::toString() const {
    std::stringstream ss;
    ss << "ElementNode(" << tag_name_ << ")";
    return ss.str();
}

// TextNode implementation
TextNode::TextNode(const std::string& text)
    : CHTLBaseNode(NodeType::TEXT), text_(text) {
}

TextNode::~TextNode() {
}

void TextNode::setText(const std::string& text) {
    text_ = text;
}

const std::string& TextNode::getText() const {
    return text_;
}

std::string TextNode::generateCode() const {
    return text_;
}

std::string TextNode::toString() const {
    std::stringstream ss;
    ss << "TextNode(\"" << text_ << "\")";
    return ss.str();
}

// CommentNode implementation
CommentNode::CommentNode(const std::string& comment)
    : CHTLBaseNode(NodeType::COMMENT), comment_(comment), generator_comment_(false) {
}

CommentNode::~CommentNode() {
}

void CommentNode::setComment(const std::string& comment) {
    comment_ = comment;
}

const std::string& CommentNode::getComment() const {
    return comment_;
}

void CommentNode::setGeneratorComment(bool generator) {
    generator_comment_ = generator;
}

bool CommentNode::isGeneratorComment() const {
    return generator_comment_;
}

std::string CommentNode::generateCode() const {
    if (generator_comment_) {
        // 生成器注释根据上下文生成不同语言的注释
        return "<!-- " + comment_ + " -->";
    } else {
        return "<!-- " + comment_ + " -->";
    }
}

std::string CommentNode::toString() const {
    std::stringstream ss;
    ss << "CommentNode(\"" << comment_ << "\")";
    return ss.str();
}

} // namespace CHTL